/*
 * CanEmulator.c
 *
 *  Created on: Jun 18, 2025
 *      Author: Doraemonjayo
 */

#include "CanEmulator.h"
#include <stddef.h>
#include <string.h>

void CanEmulator_transmit(uint32_t id, const uint8_t *data, uint8_t dlc, bool isExtended, bool isRemote, void (*txFunc)(const uint8_t *buf, uint32_t len)) {
    if (txFunc == NULL) return;
    if (dlc > 8) return;

    uint8_t buf[13] = {0};

    buf[0] = 0;
    buf[0] |= isExtended ? (0x01 << 6) : 0;
    buf[0] |= isRemote ? (0x01 << 5) : 0;
    buf[0] |= dlc;

    uint8_t index;
    if (!isExtended) {
        buf[1] = (id >> 8) & 0x07;
        buf[2] = id & 0xFF;
        index = 3;
    } else {
        buf[1] = (id >> 24) & 0x1F;
        buf[2] = (id >> 16) & 0xFF;
        buf[3] = (id >> 8) & 0xFF;
        buf[4] = id & 0xFF;
        index = 5;
    }

    if (!isRemote) {
        if (data != NULL && dlc > 0) {
            memcpy(&buf[index], data, dlc);
        }
        index += dlc;
    }

    txFunc(buf, index);
}

void CanEmulator_rxTask(const uint8_t *buf, uint32_t len, void (*rxCallback)(uint32_t id, uint8_t *data, uint8_t dlc, bool isExtended, bool isRemote)) {
    if (rxCallback == NULL) return;
    if (len < 1) return;

    bool isExtended = (buf[0] & (0x01 << 6)) ? true : false;
    bool isRemote = (buf[0] & (0x01 << 5)) ? true : false;
    uint8_t dlc = buf[0] & 0x0F;
    if (dlc > 8) return;

    uint8_t index;
    uint32_t id;
    if (!isExtended) {
        if (len < 3) return;

        id = (buf[1] & 0x07) << 8;
        id |= buf[2] & 0xFF;
        index = 3;
    } else {
        if (len < 5) return;

        id = (buf[1] & 0x1F) << 24;
        id |= (buf[2] & 0xFF) << 16;
        id |= (buf[3] & 0xFF) << 8;
        id |= buf[4] & 0xFF;
        index = 5;
    }

    uint8_t data[8] = {0};
    if (!isRemote) {
        if (len < index + dlc) return;

        memcpy(data, &buf[index], dlc);
        index += dlc;
    }

    rxCallback(id, data, dlc, isExtended, isRemote);
}
