#ifndef _MAIN_H_
#define _MAIN_H_

#include "stm32f0xx.h"
#include <stdint.h>

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

enum eAmplitude {
	now, old1
};

typedef struct {
	uint16_t amplitude;
	uint16_t scriptAmplitude[SCRIPT_POINTS];
	uint8_t controlByte;
} device;

extern device devInfo;

#endif /* _MAIN_H_ */
