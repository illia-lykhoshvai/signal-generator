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

enum eControlBits {
	halfSine = 0x01,
	runScript = 0x80
};

typedef struct scriptData {
	uint16_t amplitude[SCRIPT_POINTS];
	uint16_t time[SCRIPT_POINTS];

	uint32_t crc;
} scriptData_t;

typedef struct {
	uint16_t amplitude;
	scriptData_t scriptPoint;
	uint8_t controlByte;
} device;

extern device devInfo;
extern char msg[100];

#endif /* _MAIN_H_ */
