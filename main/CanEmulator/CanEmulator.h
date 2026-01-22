/*
 * CanEmulator.h
 *
 *  Created on: Jun 18, 2025
 *      Author: Doraemonjayo
 */

#ifndef CANEMULATOR_CANEMULATOR_H_
#define CANEMULATOR_CANEMULATOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

void CanEmulator_transmit(uint32_t id, const uint8_t *data, uint8_t dlc, bool isExtended, bool isRemote, void (*txFunc)(const uint8_t *buf, uint32_t len));
void CanEmulator_rxTask(const uint8_t *buf, uint32_t len, void (*rxCallback)(uint32_t id, uint8_t *data, uint8_t dlc, bool isExtended, bool isRemote));

#ifdef __cplusplus
}
#endif

#endif /* CANEMULATOR_CANEMULATOR_H_ */
