/*
 * can_controller.c
 *
 *  Created on: Dec 18, 2025
 *      Author: Doraemonjayo
 */

#include "main_app.h"

#define CAN_QUEUE_CAPACITY 64

static Queue can1_txQueue;
static CanPacket can1_txBuffer[CAN_QUEUE_CAPACITY];
static Queue can2_txQueue;
static CanPacket can2_txBuffer[CAN_QUEUE_CAPACITY];
static Queue cancdc_txQueue;
static CanPacket cancdc_txBuffer[CAN_QUEUE_CAPACITY];

void can_controller_init() {
	Queue_init(&can1_txQueue, can1_txBuffer, sizeof(CanPacket), CAN_QUEUE_CAPACITY, true, disable_irq_nest, enable_irq_nest);
	Queue_init(&can2_txQueue, can2_txBuffer, sizeof(CanPacket), CAN_QUEUE_CAPACITY, true, disable_irq_nest, enable_irq_nest);
	Queue_init(&cancdc_txQueue, cancdc_txBuffer, sizeof(CanPacket), CAN_QUEUE_CAPACITY, true, disable_irq_nest, enable_irq_nest);

	can1_start(&CAN1_FILTER_DEFAULT);
	can2_start(&CAN2_FILTER_DEFAULT);
	cdc_controller_init();
}

void can_controller_poll() {
	CanPacket canPacket;
	if (Queue_size(&can1_txQueue) > 0 && can1_txAvailable() > 0) {
		if (Queue_pop(&can1_txQueue, &canPacket) == 0) {
			can1_transmit(canPacket.id, canPacket.data, canPacket.dlc, canPacket.isExtended, canPacket.isRemote);
		}
	}
	if (Queue_size(&can2_txQueue) > 0 && can2_txAvailable() > 0) {
		if (Queue_pop(&can2_txQueue, &canPacket) == 0) {
			can2_transmit(canPacket.id, canPacket.data, canPacket.dlc, canPacket.isExtended, canPacket.isRemote);
		}
	}
	if (Queue_size(&cancdc_txQueue) > 0 && cdc_txAvailable() > 0) {
		if (Queue_pop(&cancdc_txQueue, &canPacket) == 0) {
			cancdc_transmit(canPacket.id, canPacket.data, canPacket.dlc, canPacket.isExtended, canPacket.isRemote);
		}
	}
}

void can1_transmitQueue(uint32_t id, const uint8_t *data, uint8_t dlc, bool isExtended, bool isRemote) {
	if (dlc > 8) return;

	CanPacket packet;
	packet.id = id;
	memcpy(packet.data, data, dlc);
	packet.dlc = dlc;
	packet.isExtended = isExtended;
	packet.isRemote = isRemote;

	Queue_push(&can1_txQueue, &packet);
}

void can2_transmitQueue(uint32_t id, const uint8_t *data, uint8_t dlc, bool isExtended, bool isRemote) {
	if (dlc > 8) return;

	CanPacket packet;
	packet.id = id;
	memcpy(packet.data, data, dlc);
	packet.dlc = dlc;
	packet.isExtended = isExtended;
	packet.isRemote = isRemote;

	Queue_push(&can2_txQueue, &packet);
}

void cancdc_transmitQueue(uint32_t id, const uint8_t *data, uint8_t dlc, bool isExtended, bool isRemote) {
	if (dlc > 8) return;

	CanPacket packet;
	packet.id = id;
	memcpy(packet.data, data, dlc);
	packet.dlc = dlc;
	packet.isExtended = isExtended;
	packet.isRemote = isRemote;

	Queue_push(&cancdc_txQueue, &packet);
}
