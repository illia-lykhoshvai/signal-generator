/*
 * hw.c
 *
 *  Created on: 6 Dec. 2022
 *      Author: illialykhoshvai
 */

#include "hw.h"

void rccInit(void);
void gpioInit(void);
void dacInit(void);
void pwmInit(void);
void encoderInit(void);
void uartInit(void);

void hwInit(void) {
	rccInit();

	RCC->AHBENR |= RCC_AHBENR_DMAEN;

	gpioInit();
	dacInit();
	pwmInit();
	encoderInit();
//	uartInit();

	RCC->AHBENR |= RCC_AHBENR_CRCEN;
}

// RCC input = HSI, 8Mhz -> PLL -> 48Mhz
void rccInit(void) {
	// HSI(8MHz) on
	RCC->CR |= RCC_CR_HSION;
	RCC->CR &= ~RCC_CR_PLLON;
	while(RCC->CR & RCC_CR_PLLRDY);
	// Set HSI as PLL src + PLLMul to x12
	RCC->CFGR |= RCC_CFGR_PLLSRC_HSI_DIV2 + RCC_CFGR_PLLMUL12;
	// PLL turn on
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY));
	// System clock mux
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	// AHB div to 1
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
	// APB div to 1
	RCC->CFGR |= RCC_CFGR_PPRE_DIV1;
	// check HSI as system clock flag ready
	while(((RCC->CFGR & RCC_CFGR_SWS_0) && (RCC->CFGR & RCC_CFGR_SWS_1)) != RCC_CFGR_SWS_HSI);
	// set system variable SystemCoreClock to current clock value
	SystemCoreClockUpdate();
}

void gpioInit(void) {
	// PA4 = DAC_OUT1
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	// analog mode
	GPIOA->MODER |= GPIO_MODER_MODER4;
	// PA0 = USR BTN with pull up already
	// PC9 = built-in LED, push-pull output
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	GPIOC->MODER |= GPIO_MODER_MODER9_0;

	// TIM1 PA8..11 = [AF2]
	GPIOA->AFR[1] |= (2 << GPIO_AFRH_AFSEL8_Pos) + (2 << GPIO_AFRH_AFSEL9_Pos)
			+ (2 << GPIO_AFRH_AFSEL10_Pos) + (2 << GPIO_AFRH_AFSEL11_Pos);
	GPIOA->MODER |= GPIO_MODER_MODER8_1 + GPIO_MODER_MODER9_1 +
			GPIO_MODER_MODER10_1 + GPIO_MODER_MODER11_1;

	// PA9 = USART1_TX
	// PA10 = USART1_RX
//	GPIOA->AFR[1] |= (1 << GPIO_AFRH_AFSEL9_Pos) + (1 << GPIO_AFRH_AFSEL10_Pos);
//	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR9_0;
//	GPIOA->MODER &= ~(GPIO_MODER_MODER9 + GPIO_MODER_MODER10);
//	GPIOA->MODER |= (GPIO_MODER_MODER9_1 + GPIO_MODER_MODER10_1);
}

void dacInit(void) {
	RCC->APB1ENR |= RCC_APB1ENR_DACEN+RCC_APB1ENR_TIM14EN;

	// software trigger + dac enable
	DAC->CR |= (DAC_CR_TSEL1_Msk + DAC_CR_EN1);

	TIM14->PSC = (SystemCoreClock/1000000)-1;
	TIM14->ARR = (M2U(PERIOD)/STEPS)-1;
	TIM14->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM14_IRQn);
	TIM14->CR1 |= TIM_CR1_CEN;
}


void pwmInit(void) {
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	//	input clock = 48mhz
	//	TIM15->ARR = (4096/PWM_DIV) - 1; // frequency
	TIM1->ARR = (4096/PWM_DIV) - 1; // frequency

	TIM1->CCR1 = 0; // duty cycle for start
	TIM1->CCR2 = 0;
	TIM1->CCMR1 |= (6 << TIM_CCMR1_OC1M_Pos) + TIM_CCMR1_OC1PE +
			(6 << TIM_CCMR1_OC2M_Pos) + TIM_CCMR1_OC2PE;// out.compare1&2 in pwm mode 1, preload enable

	TIM1->CCR3 = 0;
	TIM1->CCR4 = 0;
	TIM1->CCMR2 |= (6 << TIM_CCMR2_OC3M_Pos) + TIM_CCMR2_OC3PE +
			(6 << TIM_CCMR2_OC4M_Pos) + TIM_CCMR2_OC4PE;// out.compare3&4 in pwm mode 1, preload enable

	TIM1->CCER |= TIM_CCER_CC1E + TIM_CCER_CC2E + TIM_CCER_CC3E + TIM_CCER_CC4E; // cap/comp.1 enable
	TIM1->BDTR |= TIM_BDTR_MOE; // main output enable
	TIM1->DIER |= 0x1E00; // enable DMA on all 4 channels
	TIM1->CR1 |= TIM_CR1_CEN; // counter enable
	TIM1->EGR |= TIM_EGR_UG; // generate update

	// dma transfer
	DMA1_Channel5->CPAR = (uint32_t)&TIM1->CCR1;
	DMA1_Channel5->CMAR = (uint32_t)pwmBuffer;
	DMA1_Channel5->CNDTR = CHANNELS;
	DMA1_Channel5->CCR = (3 << DMA_CCR_PL_Pos) + DMA_CCR_MSIZE_0 + DMA_CCR_PSIZE_1
			+ DMA_CCR_MINC + DMA_CCR_PINC + DMA_CCR_DIR
			+ DMA_CCR_CIRC + DMA_CCR_EN;
}

void encoderInit(void) {
// tim3 is used in quad.encoder mode
// AF1 = tim3_ch1 pin (PA6) + tim3_ch2 pin (PA7)
	GPIOA->AFR[0] |= (1 << GPIO_AFRL_AFRL6_Pos) + (1 << GPIO_AFRL_AFRL7_Pos);
	GPIOA->MODER &= ~(GPIO_MODER_MODER6 + GPIO_MODER_MODER7);
	GPIOA->MODER |= (GPIO_MODER_MODER6_1 + GPIO_MODER_MODER7_1);

	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	// CC1 & CC2 channels as inputs, IC1 mapped to TI1, IC2 mapped to TI2
	TIM3->CCMR1 |= (TIM_CCMR1_CC1S_0 + TIM_CCMR1_CC2S_0);
	// polarity: on noninverted/rising edge
	TIM3->CCER &= ~(TIM_CCER_CC1P + TIM_CCER_CC2P);
	// select encoder mode 1
	TIM3->SMCR |= (1 << TIM_SMCR_SMS_Pos);
	// filter for jittering
	TIM3->CCMR1 |= (3 << TIM_CCMR1_IC1F_Pos);
	// counter top value
	TIM3->ARR = ENC_MAX;
	// lastly - enabling counter
	TIM3->CR1 |= TIM_CR1_CEN;
}

void uartInit(void) {
	// 48 Mhz input
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	USART1->BRR = 0x1388; // 9600 baud

//	 rx dma
	USART1->CR3 |= USART_CR3_DMAR;
	DMA1_Channel3->CPAR = (uint32_t)&USART1->RDR;
	DMA1_Channel3->CMAR = (uint32_t)rxBuffer;
	DMA1_Channel3->CNDTR = DMA_R_SIZE;
	DMA1_Channel3->CCR = (2 << DMA_CCR_PL_Pos) + DMA_CCR_MINC + DMA_CCR_CIRC + DMA_CCR_TCIE + DMA_CCR_EN;

//	 tx dma
	USART1->CR3 |= USART_CR3_DMAT;
	DMA1_Channel2->CPAR = (uint32_t)&USART1->TDR;
	DMA1_Channel2->CMAR = (uint32_t)txBuffer;
	DMA1_Channel2->CCR = (1 << DMA_CCR_PL_Pos) + DMA_CCR_MINC + DMA_CCR_DIR + DMA_CCR_TCIE;

	NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

	USART1->CR1 |= USART_CR1_RE + USART_CR1_TE + USART_CR1_UE;
}

// matches with crc32
uint32_t getCRC(uint32_t* src, uint32_t length) {
	//reset hardware module
	CRC->CR = (CRC_CR_REV_OUT + CRC_CR_REV_IN + CRC_CR_RESET);

	for (uint32_t i = 0; i < (length / sizeof(uint32_t)); i++){
		CRC->DR = *src;
		src++;
	}

	return (~CRC->DR);
}
