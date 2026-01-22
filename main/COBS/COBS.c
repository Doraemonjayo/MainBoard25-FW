/*
 * COBS.c
 *
 *  Created on: Jul 13, 2025
 *      Author: doradora
 */

#include "COBS.h"
#include <stddef.h>

int8_t COBS_init(COBS *cobs, void (*txFunc)(const uint8_t *buf, uint32_t len), void (*rxCallback)(uint8_t *buf, uint32_t len)) {
	if (cobs == NULL) return -1;

	cobs->index = -1;
	cobs->nextZero = 0;
	cobs->txFunc = txFunc;
	cobs->rxCallback = rxCallback;
	return 0;
}

int8_t COBS_decode(COBS *cobs, uint8_t data) {
	if (cobs == NULL) return -1;

	if (cobs->index < 0) {
		if (data == 0) {
			return -1;
		}
		cobs->index = 0;
		cobs->nextZero = data - 1;

		if (cobs->nextZero >= COBS_MAX_SIZE) {
			cobs->index = -1;
			cobs->nextZero = 0;
			return -1;
		}
		return 0;
	}

	if (data == 0) {
		if (cobs->index != cobs->nextZero) { //データの整合性
			cobs->index = -1;
			cobs->nextZero = 0;
			return -1;
		}
		if (cobs->rxCallback) {
			cobs->rxCallback(cobs->rxBuf, cobs->index);
		}
		cobs->index = -1;
		cobs->nextZero = 0;
		return 1;
	}

	if (cobs->index >= COBS_MAX_SIZE) {
		cobs->index = -1;
		cobs->nextZero = 0;
		return -1;
	}

	if (cobs->index == cobs->nextZero) {
		cobs->rxBuf[cobs->index++] = 0;
		cobs->nextZero = cobs->index + data - 1;

		if (cobs->nextZero >= COBS_MAX_SIZE) {
			cobs->index = -1;
			cobs->nextZero = 0;
			return -1;
		}
	} else {
		cobs->rxBuf[cobs->index++] = data;
	}

	return 0;
}

int8_t COBS_encode(COBS *cobs, const uint8_t *data, uint8_t len) {
	if (cobs == NULL) return -1;
	if (len > COBS_MAX_SIZE) return -1;

	uint8_t txBuf[COBS_MAX_SIZE + 2];
	uint8_t lastZeroIndex = 0;
	uint8_t writeIndex = 1;

	// 最初のコードバイトのプレースホルダー
	txBuf[0] = 0;

	for (uint8_t i = 0; i < len; i++) {
		if (data[i] == 0) {
			// ゼロが見つかった場合、距離を記録
			txBuf[lastZeroIndex] = writeIndex - lastZeroIndex;
			lastZeroIndex = writeIndex;
			writeIndex++;
			txBuf[lastZeroIndex] = 0; // プレースホルダー
		} else {
			// 非ゼロデータをコピー
			txBuf[writeIndex] = data[i];
			writeIndex++;
		}
	}

	// 最後のコードバイト
	txBuf[lastZeroIndex] = writeIndex - lastZeroIndex;

	// 終了ゼロバイト
	txBuf[writeIndex] = 0;
	writeIndex++;

	if (cobs->txFunc) {
		cobs->txFunc(txBuf, writeIndex);
	}

	return 0;
}
