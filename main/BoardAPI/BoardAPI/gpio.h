/*
 * gpio.h
 *
 *  Created on: Jun 8, 2025
 *      Author: Doraemonjayo
 */

#ifndef BOARDAPI_BOARDAPI_GPIO_H_
#define BOARDAPI_BOARDAPI_GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

bool gpio_getUsbId();
bool gpio_getUsrBtn();

void gpio_setLedR(bool state);
void gpio_setLedG(bool state);
void gpio_setLedB(bool state);

void gpio_setUsrBtnPressedCallback(void (*callback)());
void gpio_setUsrBtnReleasedCallback(void (*callback)());

void (*gpio_getUsrBtnPressedCallback())();
void (*gpio_getUsrBtnReleasedCallback())();

#ifdef __cplusplus
}
#endif

#endif /* BOARDAPI_BOARDAPI_GPIO_H_ */
