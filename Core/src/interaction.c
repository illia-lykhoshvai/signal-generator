/*
 * interaction.c
 *
 *  Created on: 6 Dec. 2022
 *      Author: illialykhoshvai
 */

#include "interaction.h"
#include "hw.h"

static uint16_t buttonCnt, timeCnt;

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
			devInfo.scriptAmplitude[0] = devInfo.amplitude;
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
	keyReaction();

	if (devInfo.controlByte & runScript) {
		timeCnt++;
		if (timeCnt < SCRIPT_TIME) {
			devInfo.amplitude = SCRIPT_AMPLITUDE;
		} else {
			devInfo.controlByte &= ~runScript;
			timeCnt = 0;
			devInfo.amplitude = devInfo.scriptAmplitude[0];
		}
	} else { // else = if not script
		encoderReaction();
	}
}
