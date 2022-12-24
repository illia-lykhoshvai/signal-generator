/*
 * llist.c
 *
 *  Created on: 18 Dec. 2022
 *      Author: illia
 */

#include "llist.h"

/* insert in front position */
uint8_t llistPush(node_t** head, NODE_DATA_TYPE* newPacket) {
	// allocate new linked-list member
	node_t* new = (node_t*)malloc(sizeof(node_t));

	if (new != NULL) {
		// put new data in new member
		memcpy(&(new->data), newPacket, sizeof(NODE_DATA_TYPE));

		// make new node LAST in list: next == NULL
		new->next = (*head);

		// move the head to point to the new node
		(*head) = new;

		// return success
		return 1;
	}
	// return fail
	return 0;
}

/* inserts at last position*/
uint8_t llistAppend(node_t** head, NODE_DATA_TYPE* newPacket) {
	// allocate new linked-list member
	node_t* new  = (node_t*)malloc(sizeof(node_t));
	if (new != NULL) {
		// used in step 5
		node_t* last = *head;
		// put new data in new member
		memcpy(&(new->data), newPacket, sizeof(NODE_DATA_TYPE));

		// make this new node LAST in list: next == NULL
		new->next = NULL;

		// if list is empty, than make new node as head
		if (*head == NULL) {
			*head = new;
			// return success
			return 1;
		}

		// else traverse till the last node
		while(last->next != NULL) {
			last = last->next;
		}

		// change the next of last node
		last->next = (void*)new;

		// return success
		return 1;
	}
	// return fail
	return 0;
}

void llistDelete(node_t** head, uint8_t pos) {
	node_t* temp;
	node_t* prev;

	if (head == NULL){
		return;
	}

	temp = *head;
	prev = *head;

	for (uint8_t i = 0; i < pos; i++) {
		if (i == 0 && pos == 1) {
			*head = (*head)->next;
			free(temp);
		} else {
			if ( (i == pos - 1) && temp) {
				prev->next = temp->next;
				free(temp);
				return;
			} else {
				prev = temp;

				// Position was greater than
				// number of nodes in the list
				if (prev == NULL) {
					break;
				}
				temp = temp->next;
			}
		}
	}
}

void llistDeleteLast(node_t** head) {
	node_t* temp;
	node_t* prev;

	if (head == NULL){
		return;
	}

	temp = *head;
	prev = *head;

	while(temp->next != NULL) {
		prev = temp;
		temp = temp->next;
	}

	prev->next = NULL;

	if (temp == prev) { // if deleting first node
		*head = NULL;
	}
	free(temp);
}
