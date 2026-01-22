/*
 * main_app.h
 *
 *  Created on: Oct 16, 2025
 *      Author: Doraemonjayo
 */

#ifndef MAIN_APP_H_
#define MAIN_APP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "BoardAPI.h"
#include "RoboMaster.h"
#include "Queue.h"
#include "COBS.h"
#include "CanEmulator.h"

#define FLASH_DATA_LEN 1
#define BOARD_ID_MAX 30
#define BOARD_ID_BROADCAST 31

typedef struct {
	uint32_t id;
	uint8_t data[8];
	uint8_t dlc;
	bool isExtended;
	bool isRemote;
} CanPacket;

extern RoboMasters robomasters;
extern uint8_t boardID;

void can_controller_init();
void can_controller_poll();
void can1_transmitQueue(uint32_t id, const uint8_t *data, uint8_t dlc, bool isExtended, bool isRemote);
void can2_transmitQueue(uint32_t id, const uint8_t *data, uint8_t dlc, bool isExtended, bool isRemote);
void cancdc_transmitQueue(uint32_t id, const uint8_t *data, uint8_t dlc, bool isExtended, bool isRemote);
void cdc_controller_init();
void cancdc_transmit(uint32_t id, const uint8_t *data, uint8_t dlc, bool isExtended, bool isRemote);
void cancdc_setReceivedCallback(void (*callback)(uint32_t id, uint8_t *data, uint8_t dlc, bool isExtended, bool isRemote));
void (*cancdc_getReceivedCallback())(uint32_t id, uint8_t *data, uint8_t dlc, bool isExtended, bool isRemote);
void id_controller_init();
void onUsrBtnPressed();
void setupBoardID();
uint8_t getBoardID();

#ifdef __cplusplus
}
#endif

#endif /* MAIN_APP_H_ */
