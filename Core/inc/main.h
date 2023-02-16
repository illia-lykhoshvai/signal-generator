#ifndef _MAIN_H_
#define _MAIN_H_

#include "stm32f0xx.h"
#include <stdint.h>

#include "hw.h"
#include "interaction.h"
#include "protocol.h"

#define SCRIPT_TIME (2*SECOND-(SECOND/10))
#define SCRIPT_AMPLITUDE (4000)

#define MAX_AMPLITUDE 0xfff
#define AMPLITUDE (2000)
#if (AMPLITUDE > MAX_AMPLITUDE) || (SCRIPT_AMPLITUDE > MAX_AMPLITUDE)
#error "ADC amplitude can't be bigger than 0xffff"
#endif

#define STEPS 100
#define PERIOD 20 // in ms
#define M2U(a) (a*1000)

#define MS_TRIG (STEPS/PERIOD)

#define SECOND 1000

#define SCRIPT_POINTS 10

#define CHANNELS 4

enum eControlBits {
	halfSine1 = 0x01,
	midSine1 = 0x02,
	mode1 = halfSine1 + midSine1,
	halfSine2 = 0x04,
	midSine2 = 0x08,
	mode2 = halfSine2 + midSine2,
	halfSine3 = 0x10,
	midSine3 = 0x20,
	mode3 = halfSine3 + midSine3,
	halfSine4 = 0x40,
	midSine4 = 0x80,
	mode4 = halfSine4 + midSine4
};

enum eRunScripts {
	runScript1 = 0x01,
	runScript2 = 0x02,
	runScript3 = 0x04,
	runScript4 = 0x08
};

typedef struct mainData {
	uint16_t amplitude[CHANNELS];
	uint16_t phase[CHANNELS];

	uint16_t scriptAmplitude[SCRIPT_POINTS];
	uint16_t time[SCRIPT_POINTS];

	uint32_t crc;
} workData_t;

typedef struct {
	uint32_t flashCounter;
	workData_t workData;
	uint8_t controlByte;
	uint8_t runScript;
	uint8_t currChannel;
} device;

extern device devInfo;
extern char msg[100];
extern volatile uint16_t pwmBuffer[CHANNELS];

#endif /* _MAIN_H_ */
