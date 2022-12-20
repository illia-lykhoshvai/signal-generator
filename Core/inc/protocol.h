/*
 * protocol.h
 *
 *  Created on: 12 Dec. 2022
 *      Author: illia
 */

#ifndef INC_PROTOCOL_H_
#define INC_PROTOCOL_H_

#include "stm32f0xx.h"
#include "main.h"

#define PACKET_READ 0xAA // returns packetType described data
#define PACKET_WRITE 0xBB // writes new packetType data
#define PACKET_RESPONSE 0xCC

#define PACKET_Q_SIZE 10

enum ePacketTypes {
	amplitudeOperation,
	scriptPointOperation,
	controlByteOperation,
	responseOperation
};

#define DATA_BYTES 6
typedef struct packet {
	uint8_t header;
	uint8_t packetType;
	uint8_t data[DATA_BYTES];
	uint32_t crc;
} packet_t;

#define DMA_R_SIZE (sizeof(packet_t))
extern uint8_t rxBuffer[DMA_R_SIZE];
extern char txBuffer[100];

typedef uint8_t countingSemaphore;

#endif /* INC_PROTOCOL_H_ */
