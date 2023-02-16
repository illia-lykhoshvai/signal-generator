/*
 * interaction.c
 *
 *  Created on: 6 Dec. 2022
 *      Author: illialykhoshvai
 */

#include "interaction.h"
#include "hw.h"

static uint16_t buttonCnt, timeCnt, oldAmplitude[CHANNELS];
static uint8_t altParameter = 0;

//------------------------------------------------
#define RESET_KEY_STATE 0xff
static uint8_t pressedKeys = RESET_KEY_STATE;
void pollKeys() {
	static uint8_t oldKeys = 0;
	uint8_t newKeys[3] = {0};
	for (uint8_t i = 0; i < sizeof(newKeys); i++) {
		// on-board button
		if (GPIOA->IDR & GPIO_IDR_0) {
			buttonCnt++;
			newKeys[i] |= keyBoard;
		}

		// encoder button
		if ((GPIOA->IDR & GPIO_IDR_5) == 0) {
			newKeys[i] |= keyEncoder;
		}
	}
	uint8_t resultKey = newKeys[0];
	for (uint8_t i = 1; i < sizeof(newKeys); i++) {
		resultKey &= newKeys[i];
	}

	pressedKeys = resultKey & oldKeys;
	oldKeys = resultKey;
}

uint8_t getKeys() {
	if (pressedKeys == RESET_KEY_STATE) {
		pollKeys();
		pollKeys();
	}
	return pressedKeys;
}

void changeMode(uint8_t channel) {
	if (devInfo.controlByte & (halfSine1 << 2*channel)) {
		devInfo.controlByte &= ~(mode1 << 2*channel);
		devInfo.controlByte |= (midSine1 << 2*channel);
	} else if (devInfo.controlByte & (midSine1 << 2*channel)) {
		devInfo.controlByte &= ~(mode1 << 2*channel);
		devInfo.controlByte |= (halfSine1 << 2*channel);
	}
}

//------------------------------------------------
void keyReaction() {
	static uint8_t key, oldKey;

	key = getKeys();
	if (!key && oldKey) { // press release moment
		if (oldKey == keyEncoder) {
			if (buttonCnt > (2*SECOND)) { // long press
				changeMode(devInfo.currChannel);
			} else if (buttonCnt > 5) { // press
				devInfo.runScript |= (runScript1 << devInfo.currChannel);
				oldAmplitude[0] = devInfo.workData.amplitude[0];
				timeCnt = 0;
			}
		} else if (oldKey == keyBoard) {
			if (buttonCnt > 2*SECOND) {
				// # change parameter
				altParameter ^= (1 << devInfo.currChannel);
			} else if (buttonCnt) {
				devInfo.currChannel++;
				devInfo.currChannel %= CHANNELS;
			}
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
		if ((altParameter & (1 << devInfo.currChannel)) == 0) {
			if (diff > 0) { // positive change
				if (devInfo.workData.amplitude[devInfo.currChannel] <= (0xfff-ENCODER_STEP))
					devInfo.workData.amplitude[devInfo.currChannel] += ENCODER_STEP;
			} else { // negative change
				if (devInfo.workData.amplitude[devInfo.currChannel] >= ENCODER_STEP)
					devInfo.workData.amplitude[devInfo.currChannel] -= ENCODER_STEP;
			}
		} else {
			if (diff > 0) { // positive change
				if (devInfo.workData.phase[devInfo.currChannel] <= (50-1))
					devInfo.workData.phase[devInfo.currChannel] += 1;
			} else { // negative change
				if (devInfo.workData.phase[devInfo.currChannel] >= 1)
					devInfo.workData.phase[devInfo.currChannel] -= 1;
			}
		}
		oldVal = newVal;
	}
}

void interfaceInteraction(void) {
	static uint8_t scriptCounter = 0;
	keyReaction();

	if (devInfo.runScript & (runScript1 << devInfo.currChannel)) {
		timeCnt++;
		if (timeCnt < devInfo.workData.time[scriptCounter]) {
			devInfo.workData.amplitude[devInfo.currChannel] = devInfo.workData.scriptAmplitude[scriptCounter];
		} else {
			timeCnt = 0;
			scriptCounter++;
			if ((devInfo.workData.scriptAmplitude[scriptCounter] == 0)
					|| (devInfo.workData.time[scriptCounter] == 0)) {
				devInfo.runScript &= ~(runScript1 << devInfo.currChannel);
				devInfo.workData.amplitude[0] = oldAmplitude[0];
				scriptCounter = 0;
			}
		}
	} else { // else = if not script
		encoderReaction();
	}
}

void showCurrentState(uint8_t st) {
	switch(st) {
	case 0:
		if(devInfo.flashCounter > 100) {
			devInfo.flashCounter = 0;
			GPIOC->ODR ^= GPIO_ODR_8;
		}
		break;
	case 1:
		if(devInfo.flashCounter > 500) {
			devInfo.flashCounter = 0;
			GPIOC->ODR ^= GPIO_ODR_8;
		}
		break;
	case 2:
		if(devInfo.flashCounter > 200) {
			devInfo.flashCounter = 0;
			GPIOC->ODR ^= GPIO_ODR_9;
		}
		break;
	case 3:
		if(devInfo.flashCounter > 500) {
			devInfo.flashCounter = 0;
			GPIOC->ODR ^= GPIO_ODR_9;
		}
		break;
	default:
		if(devInfo.flashCounter > 50) {
			devInfo.flashCounter = 0;
			GPIOC->ODR ^= (GPIO_ODR_8 + GPIO_ODR_9);
		}
		break;
	}
}
