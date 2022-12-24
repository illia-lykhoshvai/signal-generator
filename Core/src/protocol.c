/*
 * protocol.c
 *
 *  Created on: 12 Dec. 2022
 *      Author: illia
 */

#include "protocol.h"
#include "llist.h"

uint8_t rxBuffer[DMA_R_SIZE];
char txBuffer[DMA_T_SIZE] = {'\0'};

static node_t* qOfPackets = NULL;

void DMA1_CH2_3_DMA2_CH1_2_IRQHandler(void) {
	static packet_t localCopy;
	if (DMA1->ISR & DMA_ISR_TCIF3) { // rx buffer full
		memcpy(&localCopy, rxBuffer, sizeof(packet_t));
		DMA1->IFCR = (0xf << 8); // clear all dma_ch3 interrupts
		llistAppend(&qOfPackets, &localCopy);
	} else { // tx complete
		DMA1->IFCR = (0xf << 4); // clear all dma_ch2 interrupts
		DMA1_Channel2->CCR &= ~DMA_CCR_EN; // stop channel to restart it later
	}
}

uint8_t uartTransmit(uint8_t* src, uint16_t len) {
	if ((DMA1_Channel2->CCR & DMA_CCR_EN) == 0) {
		DMA1_Channel2->CNDTR = len;
		DMA1_Channel2->CMAR = (uint32_t)src;
		DMA1_Channel2->CCR |= DMA_CCR_EN;
		return 1;
	}
	return 0;
}

void sendPacket(uint8_t * data, enum ePacketTypes type) {
	packet_t packetToSend = {0};
	packetToSend.header = PACKET_RESPONSE;

	memcpy(packetToSend.data, data, DATA_BYTES);
	packetToSend.packetType = type;

	packetToSend.crc = getCRC((uint32_t*)&packetToSend, sizeof(packetToSend)-sizeof(packetToSend.crc));

	while( !uartTransmit((uint8_t*)&packetToSend, sizeof(packetToSend)) );
}

void analyzePacket() {
	uint32_t tmp = 0;
	packet_t packetCopy = {0};
	node_t* head = qOfPackets;

	if (head == NULL) {
		return;
	}

	while(head->next != NULL) {
		head = head->next;
	}

	memcpy(&packetCopy, &head->data, sizeof(packetCopy)); // make local copy

	tmp = getCRC((uint32_t*)&packetCopy, sizeof(packetCopy)-sizeof(packetCopy.crc));
	if (tmp == packetCopy.crc) {
		if (packetCopy.header == PACKET_READ) {
			memcpy(packetCopy.data, &devInfo.amplitude, sizeof(devInfo.amplitude));
			sendPacket(packetCopy.data, responseOperation);
		} else if (packetCopy.header == PACKET_WRITE){
			switch(packetCopy.packetType) {
			case amplitudeOperation:
				memcpy(&devInfo.amplitude, packetCopy.data, sizeof(devInfo.amplitude));
				break;
			case scriptPointOperation:
				break;
			case controlByteOperation:
				break;
			default:
				break;
			}
		} else { // nor read, nor write

		}
		llistDeleteLast(&qOfPackets);
	} else { // crc doesn't match

	}

}
