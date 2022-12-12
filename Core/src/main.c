#include "main.h"
#include "hw.h"
#include "interaction.h"

#include "math.h"

#define PI 3.1415f
#define BH_SINE(x) (16*x*(PI-x)) / ( (5*PI*PI)-4*x*(PI-x) )

volatile uint8_t msEvent = 0;
device devInfo = {0};

void TIM14_IRQHandler(void) {
	static uint16_t sineCnt = 0, msCounter = 0;
	static float sinVal = 0.00f, angle = 0.00f;

	TIM14->SR = 0;
	// Bhaskara sine approximation
	if (angle <= PI) {
		sinVal = BH_SINE(angle);
	} else {
		if (devInfo.controlByte & halfSine) {
			sinVal = 0;
		} else {
			angle -= PI;
			sinVal = BH_SINE(angle);
//			sinVal = fabs(sinVal);
		}
	}
	sinVal *= devInfo.amplitude[now];

	DAC1->DHR12R1 = (uint16_t)lroundf(sinVal);
	DAC1->SWTRIGR = DAC_SWTRIGR_SWTRIG1;

	angle = ((2*PI)/STEPS)*(sineCnt++);
//	counter++;
	sineCnt %= STEPS;

	if (++msCounter >= MS_TRIG) {
		msCounter = 0;
		msEvent = 1;
	}
}

int main(void) {
	static uint16_t hzCnt = 0;
	devInfo.amplitude[old1] = AMPLITUDE;
	devInfo.amplitude[now] = AMPLITUDE;
	devInfo.controlByte = halfSine;
	hwInit();
	/* Loop forever */
	while(1) {
		if(msEvent) {
			msEvent = 0;
			interfaceInteraction();
			if (++hzCnt >= SECOND) {
				hzCnt = 0;
				// LED TOGGLE
				GPIOC->ODR ^= GPIO_ODR_9;
			}
		} // msEvent
	} // while
} // main
