/*
 * interaction.c
 *
 *  Created on: 6 Dec. 2022
 *      Author: illialykhoshvai
 */

#include "interaction.h"
#include "hw.h"

static uint16_t buttonCnt, timeCnt, oldAmplitude;

uint8_t pollKeys() {
	if (GPIOA->IDR & GPIO_IDR_0) {
		buttonCnt++;
		return 1;
	}
	return 0;
}

void keyReaction() {
	static uint8_t key, oldKey;

	key = pollKeys();
	if (!key && oldKey) { // press release moment
		if (buttonCnt > (2*SECOND)) { // long press
			devInfo.controlByte ^= halfSine;
		} else if (buttonCnt > 5) { // press
			devInfo.controlByte |= runScript;
			oldAmplitude = devInfo.amplitude;
			timeCnt = 0;
		}
		buttonCnt = 0;
	}
	oldKey = key;
}

#define ENCODER_STEP 15
void encoderReaction(void) {
	static ENC_TYPE oldVal = 0;
	ENC_TYPE newVal = TIM3->CNT;

	if (newVal != oldVal) {
		int16_t diff = newVal - oldVal;
		if (diff > 0) { // positive change
			if (devInfo.amplitude <= (0xfff-ENCODER_STEP))
				devInfo.amplitude += ENCODER_STEP;
		} else { // negative change
			if (devInfo.amplitude >= ENCODER_STEP)
				devInfo.amplitude -= ENCODER_STEP;
		}
		oldVal = newVal;
	}
}

void interfaceInteraction(void) {
	static uint8_t scriptCounter = 0;
	keyReaction();

	if (devInfo.controlByte & runScript) {
		timeCnt++;
		if (timeCnt < devInfo.scriptPoint.time[scriptCounter]) {
			devInfo.amplitude = devInfo.scriptPoint.amplitude[scriptCounter];
		} else {
			timeCnt = 0;
			scriptCounter++;
			if ((devInfo.scriptPoint.amplitude[scriptCounter] == 0)
					|| (devInfo.scriptPoint.time[scriptCounter] == 0)) {
				devInfo.controlByte &= ~runScript;
				devInfo.amplitude = oldAmplitude;
				scriptCounter = 0;
			}
		}
	} else { // else = if not script
		encoderReaction();
	}
}
