#include "main.h"
#include "math.h"

void RCC_init(void);
void GPIO_init(void);
void DAC_init(void);

#define PI 3.1415f
#define BH_SIN(x) (16*x*(PI-x)) / ( (5*PI*PI)-4*x*(PI-x) )

volatile uint8_t msEvent = 0;
uint16_t amplitude = AMPLITUDE;

void TIM14_IRQHandler(void) {
	static uint16_t counter = 0, msCounter = 0;
	static float sinVal = 0.00f, angle = 0.00f;

	TIM14->SR = 0;
	// Bhaskara sine approximation
	if (angle <= PI) {
		sinVal = BH_SIN(angle);
	} else {
#ifdef HALFWAVE
		sinVal = 0;
#else
		angle -= PI;
		sinVal = BH_SIN(angle);
		sinVal = fabs(sinVal);
#endif
	}
	sinVal *= amplitude;

	DAC1->DHR12R1 = (uint16_t)lroundf(sinVal);
	DAC1->SWTRIGR = DAC_SWTRIGR_SWTRIG1;

	angle = ((2*PI)/STEPS)*(counter++);
//	counter++;
	counter %= STEPS;

	if (++msCounter >= MS_TRIG) {
		msCounter = 0;
		msEvent = 1;
	}
}

int main(void) {
	static uint16_t buttonC, timeCounter;
	static uint8_t key, oldKey, runScript;
	RCC_init();
	GPIO_init();
	DAC_init();
	/* Loop forever */
	while(1) {
		if(msEvent) {
			msEvent = 0;

			if (GPIOA->IDR & GPIO_IDR_0) {
				buttonC++;
				key = 1;
			} else {
				key = 0;
			}

			if (!key && oldKey) { // press release moment
				if (buttonC > (SECOND/2)) { // long press
					amplitude = AMPLITUDE;
				} else if (buttonC > 5) { // press
//					amplitude -= 200;
					runScript = 1;
					timeCounter = 0;
				}
				buttonC = 0;
			}
			oldKey = key;


			if (runScript) {
				timeCounter++;
				amplitude = 2500;
				if (timeCounter >= SECOND) {
					runScript = 0;
					timeCounter = 0;
					amplitude = AMPLITUDE;
				}
			} // runScript
		} // msEvent
	} // while
} // main

// RCC input = HSI, 8Mhz -> PLL -> 48Mhz
void RCC_init(void) {
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

void GPIO_init(void) {
	// PA4 = DAC_OUT1
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	// analog mode
	GPIOA->MODER |= GPIO_MODER_MODER4;
	// PA0 = USR BTN with pull up
}

void DAC_init(void) {
	RCC->APB1ENR |= RCC_APB1ENR_DACEN+RCC_APB1ENR_TIM14EN;

	// software trigger + dac enable
	DAC->CR |= (DAC_CR_TSEL1_Msk + DAC_CR_EN1);

	TIM14->PSC = (SystemCoreClock/1000000)-1;
	TIM14->ARR = (M2U(PERIOD)/STEPS)-1;
	TIM14->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM14_IRQn);
	TIM14->CR1 |= TIM_CR1_CEN;
}
