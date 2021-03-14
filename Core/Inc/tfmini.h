#ifndef TFMINI_H
#define TFMINI_H

#include "stm32f1xx_hal.h"
#include <stdint.h>

/* Sensor hardware parameters */
#define TFMINI_MAX_DISTANCE 			10000
#define TFMINI_MODE						/* FIXME */ 			
#define TFMINI_RESPONSE_TIMEOUT			50
#define TFMINI_RESET_PAYLOAD			0x5A040260

typedef struct {
	uint16_t 	header;
	uint16_t 	distance;
	uint16_t 	strength;
	uint16_t 	temperature;
	uint8_t 	checksum;
} tfmini_package ;

typedef struct {
	uint16_t 	frequency;
	uint16_t 	max_distance;
	uint16_t	sample_rate;
	uint8_t 	status;
} tfmini_state ;

void tfmini_init(void);
int8_t tfmini_wait(void);
void tfmini_reset(void);
tfmini_package tfmini_get_package(void);
tfmini_state tfmini_get_state(void);
void tfmini_set_state(tfmini_state state);
static uint8_t __tfmini_calculate_checksum(void);
static void __tfmini_receive_package(void);

#endif /* TFMINI_H */