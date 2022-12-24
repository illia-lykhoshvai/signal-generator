/*
 * hw.c
 *
 *  Created on: 6 Dec. 2022
 *      Author: illialykhoshvai
 */

#include "hw.h"
#include "main.h"

void rccInit(void);
void gpioInit(void);
void dacInit(void);
void encoderInit(void);

void hwInit(void) {
	rccInit();
	gpioInit();
	dacInit();
	encoderInit();
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
