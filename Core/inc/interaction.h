/*
 * interaction.h
 *
 *  Created on: 6 Dec. 2022
 *      Author: illialykhoshvai
 */

#ifndef INC_INTERACTION_H_
#define INC_INTERACTION_H_

#include "main.h"

enum Keys {
	keyNo = 0,
	keyBoard = 1,
	keyEncoder = 2
};

void pollKeys();

void interfaceInteraction(void);

void showCurrentState(uint8_t st);

#endif /* INC_INTERACTION_H_ */
