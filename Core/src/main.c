#include "main.h"

#include "string.h"
#include "math.h"

#include "stdio.h"

#define PI 3.1415f
#define BH_SINE(x) (16*x*(PI-x)) / ( (5*PI*PI)-4*x*(PI-x) )
static float sinTable[(STEPS/2)];

volatile uint8_t msEvent = 0;
device devInfo = {0};

uint16_t pwmBuffer[CHANNELS];
void TIM14_IRQHandler(void) {
	static uint16_t msCounter, sineCounter, halfSineFlag;
	uint16_t temp[CHANNELS];

	TIM14->SR = 0;
	// Bhaskara sine approximation
	for (uint8_t i = 0; i < CHANNELS; i++) {
		temp[i] = (uint16_t)lroundf(sinTable[(sineCounter + devInfo.workData.phase[i]) % (STEPS/2)]
											 *devInfo.workData.amplitude[i]);

		if (devInfo.controlByte & (halfSine1 << i)) {
			if (sineCounter == devInfo.workData.phase[i]) {
				halfSineFlag ^= (1 << i);
			}
			if (halfSineFlag & (1 << i)) {
				temp[i] = 0;
			}
		}
		pwmBuffer[i] = temp[i]/PWM_DIV;
	}
	sineCounter = (sineCounter+1) % (STEPS/2);

	// used for I
	DAC1->DHR12R1 = temp[1]; // if halfSine
	DAC1->SWTRIGR = DAC_SWTRIGR_SWTRIG1;

	if (++msCounter >= MS_TRIG) {
		msCounter = 0;
		msEvent = 1;
	}
}

uint8_t dummy[8] = {0};
int main(void) {
	static uint16_t hzCnt = 0;

	for(uint8_t i = 0; i < CHANNELS; i++) {
		devInfo.workData.amplitude[i] = AMPLITUDE;
	}
	devInfo.workData.phase[1] = 50;
	devInfo.workData.time[0] = SCRIPT_TIME;
	devInfo.workData.scriptAmplitude[0] = SCRIPT_AMPLITUDE;

	for (uint32_t i = 0; i < (STEPS/2); i++) {
		float angle = (PI/(STEPS/2))*i;
		sinTable[i] = BH_SINE(angle);
	}

	// check phase borders
	for (uint32_t i = 0; i < CHANNELS; i++) {
		if (devInfo.workData.phase[i] >= (STEPS/2)) {
			devInfo.workData.phase[i] %= (STEPS/2);
		}
		if (devInfo.workData.phase[i] < 0) {
			devInfo.workData.phase[i] = 0;
		}
	}

	hwInit();

	/* Loop forever */
	while(1) {
		if(msEvent) {
			msEvent = 0;

			interfaceInteraction();
			pollKeys();

			analyzePacket();

			devInfo.flashCounter++;
			showCurrentState(devInfo.currChannel);

			if (++hzCnt >= SECOND) {
				hzCnt = 0;
				GPIOC->ODR ^= GPIO_ODR_9;
//				if (oldAmplitude != devInfo.amplitude) {
//					oldAmplitude = devInfo.amplitude;
//					DMA1_Channel2->CNDTR = sprintf(txBuffer,"Amplitude is: %u\n", devInfo.amplitude);
//					// start dma->tx
//					DMA1_Channel2->CCR |= DMA_CCR_EN;
//				}
			}
		} // msEvent
	} // while
} // main
