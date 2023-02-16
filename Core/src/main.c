#include "main.h"

#include "string.h"
#include "math.h"

#include "stdio.h"

#define PI 3.1415f
#define BH_SINE(x) (16*x*(PI-x)) / ( (5*PI*PI)-4*x*(PI-x) )
static float sinTable[STEPS]; // one wave + another wave

volatile uint8_t msEvent = 0;
device devInfo = {0};

volatile uint16_t pwmBuffer[CHANNELS];
static volatile uint8_t msCounter, sineCounter[CHANNELS], tempFlags, durCnt;
static volatile int16_t temp;
void TIM14_IRQHandler(void) {

	TIM14->SR = 0;
	// Bhaskara sine approximation
	for (uint8_t i = 0; i < CHANNELS; i++) {
		temp = (uint16_t)lroundf(sinTable[(sineCounter[i] + devInfo.workData.phase[i])]
										   *devInfo.workData.amplitude[i]);
		if (devInfo.controlByte & (halfSine1 << (2*i))) {
			if (sineCounter[i] == devInfo.workData.phase[i]) {
				tempFlags ^= (1 << (2*i));
			}
			if (tempFlags & (1 << 2*i)) {
				temp = 0;
			}
		} else if (devInfo.controlByte & (midSine1 << (2*i))) {
			temp /= 2;
			if (sineCounter[i] == ((STEPS/2) - devInfo.workData.phase[i] - 1)) {
				tempFlags ^= (2 << (2*i));
			}
			if (tempFlags & (2 << (2*i))) { //# inverted sin
				temp = (MAX_AMPLITUDE/2) - temp;
			} else {
				temp += (MAX_AMPLITUDE/2);
			}
		}
		pwmBuffer[i] = temp/PWM_DIV;
		sineCounter[i] = (sineCounter[i] + 1) % (STEPS/2);
	}

	// used for I
	DAC1->DHR12R1 = pwmBuffer[1]; // if halfSine
	DAC1->SWTRIGR = DAC_SWTRIGR_SWTRIG1;

	if (++msCounter >= MS_TRIG) {
		msCounter = 0;
		msEvent = 1;
	}
}

int main(void) {
	static uint16_t hzCnt = 0;

	for(uint8_t i = 0; i < CHANNELS; i++) {
		devInfo.workData.amplitude[i] = AMPLITUDE;
	}
	devInfo.workData.time[0] = SCRIPT_TIME;
	devInfo.workData.scriptAmplitude[0] = SCRIPT_AMPLITUDE;
	devInfo.workData.amplitude[1] = 350;
	devInfo.workData.amplitude[2] = 1700;
	devInfo.controlByte = midSine2;
	devInfo.currChannel = 1;

	for (uint32_t i = 0; i < (STEPS); i++) {
		float angle = (PI/(STEPS/2))*i;
		if (angle > PI) {
			angle -= PI;
		}
		sinTable[i] = BH_SINE(angle);
	}

	// check phase borders
	for (uint32_t i = 0; i < CHANNELS; i++) {
		if (devInfo.workData.phase[i] >= (STEPS/2)) {
			devInfo.workData.phase[i] %= (STEPS/2);
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
