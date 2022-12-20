/*
 * hw.h
 *
 *  Created on: 6 Dec. 2022
 *      Author: illialykhoshvai
 */

#ifndef INC_HW_H_
#define INC_HW_H_

#include "main.h"

#define ENC_MAX 200
#if (ENC_MAX <= 0xff)
#define ENC_TYPE uint8_t
#else
#define ENC_TYPE uint16_t
#endif

/*
 * initializes used in project mcu peripherals
 */
void hwInit(void);

/*
 * uses CRC module to calc. CRC-32
 */
uint32_t getCRC(uint32_t* src, uint32_t length);

#endif /* INC_HW_H_ */
