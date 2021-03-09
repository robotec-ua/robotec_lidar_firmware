#ifndef TFMINI_H
#define TFMINI_H

#include "stm32f1xx_hal.h"
#include <stdint.h>

/* Sensor hardware parameters */
#define TFMINI_MAX_DISTANCE 			10000
#define TFMINI_MODE						/* FIXME */ 			
#define TFMINI_RESPONSE_TIMEOUT			50

/* Sensor package data extracting */
#define TFMINI_MASK						0xff
#define TFMINI_BLOCK_SIZE 				8
#define TFMINI_GET_HEADER(package)		(package >> (7 * TFMINI_BLOCK_SIZE))
#define TFMINI_GET_DIST(package)		((package >> (5 * TFMINI_BLOCK_SIZE)) & TFMINI_MASK)
#define TFMINI_GET_STRENGTH(package)	((package >> (3 * TFMINI_BLOCK_SIZE)) & TFMINI_MASK)
#define TFMINI_GET_TEMP(package)		((package >> (1 * TFMINI_BLOCK_SIZE)) & TFMINI_MASK)
#define TFMINI_GET_CHECKSUM(package)	(package & TFMINI_MASK)

struct __tfmini_package {
	uint16_t distance;
	uint16_t strength;
	uint16_t temperature;
};

struct __tfmini_state {
	uint8_t mode;
	uint16_t max_distance;
	uint16_t sample_rate;
	uint8_t status;
};

void tfmini_init(void);
static void __tfmini_error_handler(void);
static uint8_t __tfmini_calculate_checksum(uint64_t data);
int8_t tfmini_wait(void);
void __tfmini_unpack(uint64_t data);
uint8_t tfmini_get_sensor_status(void);
void tfmini_sensor_reset(void);
uint16_t tfmini_get_sample_rate(void);
void tfmini_set_sample_rate(uint16_t value);
uint8_t tfmini_get_mode(void);
void tfmini_set_mode(uint8_t value);
uint8_t tfmini_get_max_distance(void);
void tfmini_set_max_distance(uint16_t value);

#endif /* TFMINI_H */