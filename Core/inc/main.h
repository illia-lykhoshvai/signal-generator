#ifndef _MAIN_H_
#define _MAIN_H_

#include "stm32f0xx.h"
#include <stdint.h>

#include "hw.h"
#include "interaction.h"
#include "protocol.h"

#define SCRIPT_TIME (SECOND+(SECOND/2))
#define SCRIPT_AMPLITUDE (AMPLITUDE*2)

#define AMPLITUDE (2000)
#if (AMPLITUDE > 0xfff) || (SCRIPT_AMPLITUDE > 0xfff)
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
	halfSine2 = 0x02,
	halfSine3 = 0x04,
	halfSine4 = 0x04,
	runScript = 0x80
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
	uint8_t currChannel;
} device;

extern device devInfo;
extern char msg[100];

#endif /* _MAIN_H_ */
