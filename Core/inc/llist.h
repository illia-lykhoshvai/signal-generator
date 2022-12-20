/*
 * llist.h
 *
 *  Created on: 18 Dec. 2022
 *      Author: illia
 */

#ifndef INC_LLIST_H_
#define INC_LLIST_H_

#include "protocol.h"

#include "stdint.h"
#include "string.h"
#include "malloc.h"

#define NODE_DATA_TYPE packet_t

typedef struct node_s {
	NODE_DATA_TYPE data;
	void* next;
}node_t;

/* inserts at start position */
uint8_t llistPush(node_t** head, NODE_DATA_TYPE* newPacket);

/* inserts at last position*/
uint8_t llistAppend(node_t** head, NODE_DATA_TYPE* newPacket);

void llistDelete(node_t** head, uint8_t pos);

#endif /* INC_LLIST_H_ */
