#include "main.h"

void RCC_init(void);
void GPIO_init(void);

int main(void) {
	RCC_init();
	GPIO_init();
	/* Loop forever */
	while(1) {

	}
}

// RCC input = HSI, 8Mhz -> PLL -> 48Mhz
void RCC_init(void) {
	// HSI(8MHz) on
	RCC->CR |= RCC_CR_HSION;
	// Set HSI as PLL src
	RCC->CFGR |= RCC_CFGR_PLLSRC_HSI_DIV2;
	// Set PLLMul to x12
	RCC->CFGR |= RCC_CFGR_PLLMUL12;
	// PLL turn on
	RCC->CR |= RCC_CR_PLLON;
	// System clock mux
	RCC->CFGR |= RCC_CFGR_SWS_PLL;
	// AHB div to 1
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
	// APB div to 1
	RCC->CFGR |= RCC_CFGR_PPRE_DIV1;
	// check HSI as system clock flag ready
	while(((RCC->CFGR & RCC_CFGR_SWS_0) && (RCC->CFGR & RCC_CFGR_SWS_1)) != RCC_CFGR_SWS_HSI);
	// set system variable SystemCoreClock to current clock value
	SystemCoreClockUpdate();
	// set systick timer to 1 us delay
	SysTick_Config(SystemCoreClock/1000000);
}
void GPIO_init(void) {

}
