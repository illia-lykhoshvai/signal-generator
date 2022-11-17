#ifndef _MAIN_H_

#define _MAIN_H_

#include "stm32f0xx.h"
#include <stdint.h>

#define HALFWAVE

#define AMPLITUDE (1400)
#if (AMPLITUDE > 4095)
#error "Can't be bigger than 0xfff"
#endif

#define STEPS 100
#define PERIOD 20 // in ms
#define M2U(a) (a*1000)

#define MS_TRIG (STEPS/PERIOD)

#define SECOND 1000

#endif /* _MAIN_H_ */
