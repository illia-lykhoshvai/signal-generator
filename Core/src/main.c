#include "main.h"

#include "string.h"
#include "math.h"

#include "stdio.h"

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
		}
	}
	sinVal *= devInfo.amplitude;

	DAC1->DHR12R1 = (uint16_t)lroundf(sinVal);
	DAC1->SWTRIGR = DAC_SWTRIGR_SWTRIG1;

	angle = ((2*PI)/STEPS)*(sineCnt++);
	sineCnt %= STEPS;

	if (++msCounter >= MS_TRIG) {
		msCounter = 0;
		msEvent = 1;
	}
}

uint8_t dummy[8] = {0};
static uint16_t oldAmplitude = 0;
int main(void) {
	static uint16_t hzCnt = 0;
	devInfo.amplitude = AMPLITUDE;
	devInfo.scriptPoint.time[0] = SCRIPT_TIME;
	devInfo.scriptPoint.amplitude[0] = SCRIPT_AMPLITUDE;
	hwInit();

	/* Loop forever */
	while(1) {
		if(msEvent) {
			msEvent = 0;
			interfaceInteraction();

			analyzePacket();

			if (++hzCnt >= SECOND) {
				hzCnt = 0;
				// LED TOGGLE
				GPIOC->ODR ^= GPIO_ODR_9;
				if (oldAmplitude != devInfo.amplitude) {
					oldAmplitude = devInfo.amplitude;
					DMA1_Channel2->CNDTR = sprintf(txBuffer,"Amplitude is: %u\n", devInfo.amplitude);
					// start dma->tx
					DMA1_Channel2->CCR |= DMA_CCR_EN;
				}
			}
		} // msEvent
	} // while
} // main
