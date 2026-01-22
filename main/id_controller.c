/*
 * id_controller.c
 *
 *  Created on: Jan 23, 2026
 *      Author: Doraemonjayo
 */

#include "main_app.h"

void id_controller_init() {
	if (gpio_getUsrBtn()) {
		setupBoardID();
	}
	boardID = getBoardID();
	gpio_setUsrBtnPressedCallback(onUsrBtnPressed);
}

void onUsrBtnPressed() {
	HAL_NVIC_SystemReset();
}

void setupBoardID() {
	while (gpio_getUsrBtn());
	uint32_t lastReleased = HAL_GetTick();
	HAL_Delay(10);	// prevent chattering
	while (!gpio_getUsrBtn()) {
		if (HAL_GetTick() - lastReleased > 1000) {
			HAL_NVIC_SystemReset();
		}
	}
	HAL_Delay(10);
	while (gpio_getUsrBtn());
	lastReleased = HAL_GetTick();
	uint32_t flashData[FLASH_DATA_LEN];
	flash_read(flashData, FLASH_DATA_LEN);
	uint8_t id = 0;
	while (1)
	{
		HAL_Delay(10);
		while (!gpio_getUsrBtn()) {
			if (HAL_GetTick() - lastReleased > 1000) {
				flashData[0] = id;
				flash_write(flashData, FLASH_DATA_LEN);
				HAL_NVIC_SystemReset();
			}
		}
		id++;
		id %= (BOARD_ID_MAX + 1);
		HAL_Delay(10);
		while (gpio_getUsrBtn());
		lastReleased = HAL_GetTick();
	}
}

uint8_t getBoardID() {
	uint32_t id = FLASH_USERDATA[0];
	if (id > BOARD_ID_MAX) {
		return 0;
	} else {
		return (uint8_t)id;
	}
}
