/*
 * COBS.h
 *
 *  Created on: Jul 13, 2025
 *      Author: doradora
 */

#ifndef COBS_H_
#define COBS_H_

#define COBS_MAX_SIZE 254

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
	uint8_t rxBuf[COBS_MAX_SIZE];
	int16_t index;
	int16_t nextZero;
	void (*txFunc)(const uint8_t *buf, uint32_t len);
	void (*rxCallback)(uint8_t *buf, uint32_t len);
} COBS;

int8_t COBS_init(COBS *cobs, 	void (*txFunc)(const uint8_t *buf, uint32_t len), void (*rxCallback)(uint8_t *buf, uint32_t len));
int8_t COBS_decode(COBS *cobs, uint8_t data);
int8_t COBS_encode(COBS *cobs, const uint8_t *data, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif /* COBS_H_ */
