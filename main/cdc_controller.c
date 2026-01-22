/*
 * cdc_controller.c
 *
 *  Created on: Jan 7, 2026
 *      Author: Doraemonjayo
 */

#include "main_app.h"

static COBS cobs;
static void (*cancdc_receivedCallback)(uint32_t id, uint8_t *data, uint8_t dlc, bool isExtended, bool isRemote) = NULL;

static void cdc_rxCallback(uint8_t *buf, uint32_t len);
static void COBS_transmit(const uint8_t *buf, uint32_t len);
static void COBS_rxCallback(uint8_t *buf, uint32_t len);

void cdc_controller_init() {
	COBS_init(&cobs, cdc_transmit, COBS_rxCallback);
	cdc_setReceivedCallback(cdc_rxCallback);
}

void cancdc_transmit(uint32_t id, const uint8_t *data, uint8_t dlc, bool isExtended, bool isRemote) {
	CanEmulator_transmit(id, data, dlc, isExtended, isRemote, COBS_transmit);
}

void cancdc_setReceivedCallback(void (*callback)(uint32_t id, uint8_t *data, uint8_t dlc, bool isExtended, bool isRemote)) {
	cancdc_receivedCallback = callback;
}

void (*cancdc_getReceivedCallback())(uint32_t id, uint8_t *data, uint8_t dlc, bool isExtended, bool isRemote) {
	return cancdc_receivedCallback;
}

static void cdc_rxCallback(uint8_t *buf, uint32_t len) {
	for (uint32_t i = 0; i < len; i++) {
		COBS_decode(&cobs, buf[i]);
	}
}

static void COBS_transmit(const uint8_t *buf, uint32_t len) {
	COBS_encode(&cobs, buf, len);
}

static void COBS_rxCallback(uint8_t *buf, uint32_t len) {
	if (cancdc_receivedCallback == NULL) return;
	CanEmulator_rxTask(buf, len, cancdc_receivedCallback);
}
