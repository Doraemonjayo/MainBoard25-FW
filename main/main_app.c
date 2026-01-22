/*
 * main_app.c
 *
 *  Created on: Oct 16, 2025
 *      Author: Doraemonjayo
 */

#include "main_app.h"

RoboMasters robomasters;
uint8_t boardID = 0;

static CanPacket can1_rxPacket = {0};
static CanPacket can2_rxPacket = {0};
static CanPacket cancdc_rxPacket = {0};

static void task1kHz();
static void can1_rxCallback(uint32_t id, uint8_t *data, uint8_t dlc, bool isExtended, bool isRemote);
static void can2_rxCallback(uint32_t id, uint8_t *data, uint8_t dlc, bool isExtended, bool isRemote);
static void cancdc_rxCallback(uint32_t id, uint8_t *data, uint8_t dlc, bool isExtended, bool isRemote);

void setup() {
	timer_startUs();

	id_controller_init();

	RoboMasters_setTxFunc(&robomasters, can1_transmitQueue);
	RoboMasterConfig rmConf = {0};
	rmConf.model = ROBOMASTER_M2006;
	PID_init(&rmConf.pidVelocity, &ROBOMASTER_M2006_PID_VELOCITY_DEFAULT);
	PID_init(&rmConf.pidPosition, &ROBOMASTER_M2006_PID_POSITION_DEFAULT);
	rmConf.controlTimeout = 200;	 // [ms]
	rmConf.feedbackTimeout = 200; // [ms]
	rmConf.rawPositionOffset = 0;
	rmConf.trapezoidAcceleration = 36.0f * 2.0f * PI;
	rmConf.trapezoidMaxAcceleration = rmConf.trapezoidAcceleration * 1.5f;
	rmConf.trapezoidDeadband = PI / 8.0f;
	rmConf.getMsFunc = HAL_GetTick;
	for (uint8_t i = 0; i < 8; i++) {
		RoboMaster_init(&robomasters.robomaster[i], &rmConf);
		RoboMaster_setCurrentLimit(&robomasters.robomaster[i],
				-ROBOMASTER_M2006_MAX_CURRENT, ROBOMASTER_M2006_MAX_CURRENT);
		RoboMaster_setVelocityLimit(&robomasters.robomaster[i],
				-360.0f * 2.0f * PI, 360.0f * 2.0f * PI); // [rad/s]
		RoboMaster_setPositionLimit(&robomasters.robomaster[i],
				-INFINITY, INFINITY); // [rad]
	}

	timer_start1kHzTask();
	can_controller_init();

	timer_set1kHzTask(task1kHz);
	can1_setReceivedCallback(can1_rxCallback);
	can2_setReceivedCallback(can2_rxCallback);
	cancdc_setReceivedCallback(cancdc_rxCallback);
}

void loop() {
	can_controller_poll();
}

static void task1kHz() {
	static uint32_t tick = 0;

	gpio_setLedR((((tick / 500) % (boardID + 4)) <= boardID) && ((tick % 500) < 100));

	if (tick % 1000 == 0){
		RoboMaster_transmitAllFree(&robomasters);	// prevent stall shutdown
	}
	for (uint8_t i = 0; i < ROBOMASTER_MOTOR_NUM; i++) {
		RoboMaster_calculateOutputCurrent(&robomasters.robomaster[i]);
	}
	RoboMaster_transmit(&robomasters);

	tick++;
}

static void can1_rxCallback(uint32_t id, uint8_t *data, uint8_t dlc, bool isExtended, bool isRemote) {
	can1_rxPacket.id = id;
	memcpy(can1_rxPacket.data, data, dlc);
	can1_rxPacket.dlc = dlc;
	can1_rxPacket.isExtended = isExtended;
	can1_rxPacket.isRemote = isRemote;

	RoboMaster_rxTask(&robomasters, id, data, dlc, isExtended, isRemote);
	for (uint8_t i = 0; i < ROBOMASTER_MOTOR_NUM; i++) {
		if (robomasters.robomaster[i].feedback.temperature == 0) {
			if (robomasters.robomaster[i].config.model == ROBOMASTER_M3508) {
				PID_setConfig(&robomasters.robomaster[i].config.pidVelocity, &ROBOMASTER_M2006_PID_VELOCITY_DEFAULT);
				PID_setConfig(&robomasters.robomaster[i].config.pidPosition, &ROBOMASTER_M2006_PID_POSITION_DEFAULT);
				RoboMaster_setCurrentLimit(&robomasters.robomaster[i],
						-ROBOMASTER_M2006_MAX_CURRENT, ROBOMASTER_M2006_MAX_CURRENT);
				RoboMaster_setVelocityLimit(&robomasters.robomaster[i],
						-360.0f * 2.0f * PI, 360.0f * 2.0f * PI); // [rad/s]
				RoboMaster_setPositionLimit(&robomasters.robomaster[i],
						-INFINITY, INFINITY); // [rad]
				robomasters.robomaster[i].config.model = ROBOMASTER_M2006;
			}
		} else {
			if (robomasters.robomaster[i].config.model == ROBOMASTER_M2006) {
				PID_setConfig(&robomasters.robomaster[i].config.pidVelocity, &ROBOMASTER_M3508_PID_VELOCITY_DEFAULT);
				PID_setConfig(&robomasters.robomaster[i].config.pidPosition, &ROBOMASTER_M3508_PID_POSITION_DEFAULT);
				RoboMaster_setCurrentLimit(&robomasters.robomaster[i],
						-ROBOMASTER_M3508_MAX_CURRENT, ROBOMASTER_M3508_MAX_CURRENT);
				RoboMaster_setVelocityLimit(&robomasters.robomaster[i],
						-150.0f * 2.0f * PI, 150.0f * 2.0f * PI); // [rad/s]
				RoboMaster_setPositionLimit(&robomasters.robomaster[i],
						-INFINITY, INFINITY); // [rad]
				robomasters.robomaster[i].config.model = ROBOMASTER_M3508;
			}
		}
	}
}

static void can2_rxCallback(uint32_t id, uint8_t *data, uint8_t dlc, bool isExtended, bool isRemote) {
	can2_rxPacket.id = id;
	memcpy(can2_rxPacket.data, data, dlc);
	can2_rxPacket.dlc = dlc;
	can2_rxPacket.isExtended = isExtended;
	can2_rxPacket.isRemote = isRemote;

//	---example---
//	if (!isExtended || isRemote) {
//		return;
//	}
//	uint8_t targetID = canparser_getTargetID(id);
//	if (targetID == boardID) {
//		canparser_rxTask(id, data, dlc, isExtended, isRemote, can2_transmitQueue);	// can2_transmitQueue is for response
//	} else if (targetID == BOARD_ID_BROADCAST) {
//		canparser_rxTask(id, data, dlc, isExtended, isRemote, can2_transmitQueue);	// can2_transmitQueue is for response
//		if (cdc_isConnected()) {
//			cancdc_transmitQueue(id, data, dlc, isExtended, isRemote);
//		}
//	} else {
//		if (cdc_isConnected()) {
//			cancdc_transmitQueue(id, data, dlc, isExtended, isRemote);
//		}
//	}
}

static void cancdc_rxCallback(uint32_t id, uint8_t *data, uint8_t dlc, bool isExtended, bool isRemote) {
	cancdc_rxPacket.id = id;
	memcpy(cancdc_rxPacket.data, data, dlc);
	cancdc_rxPacket.dlc = dlc;
	cancdc_rxPacket.isExtended = isExtended;
	cancdc_rxPacket.isRemote = isRemote;

//	---example---
//	if (!isExtended || isRemote) {
//		return;
//	}
//	uint8_t targetID = canparser_getTargetID(id);
//	if (targetID == boardID) {
//		canparser_rxTask(id, data, dlc, isExtended, isRemote, cancdc_transmitQueue);	// cancdc_transmitQueue is for response
//	} else if (targetID == BOARD_ID_BROADCAST) {
//		canparser_rxTask(id, data, dlc, isExtended, isRemote, cancdc_transmitQueue);	// cancdc_transmitQueue is for response
//		can2_transmitQueue(id, data, dlc, isExtended, isRemote);
//	} else {
//		can2_transmitQueue(id, data, dlc, isExtended, isRemote);
//	}
}

const char *cdc_getProductName() {
	static char str[64];
	sprintf(str, "MainBoard25_%d", getBoardID());
	return str;
}

const char *cdc_getVendorName() {
	return "ToyotaRoboconB";
}
