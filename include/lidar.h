#ifndef PACKAGE_H
#define PACKAGE_H

/**/
#include <stdlib.h>
#include <stdint.h>

/**/
#include "stm32f1xx_hal.h"

/**/
#define LIDAR_CMD_HEADER 0x52f3

/**
 * Struct which represents the package of transmitted data
 */
typedef struct {
	uint8_t			header;
	uint8_t 		address;		// Adress of the device
	uint8_t 		command;		// Command to the device
	uint8_t 		hash;			// Hash of the package (CRC32)
	uint8_t 		size;			// Size of the package
	unsigned int 	data;			// Additional data needed by the devices
} lidar_package ;

typedef struct {
	uint8_t status;
} lidar_state;

void lidar_uart_init(void);
void lidar_spi_init(void);
void lidar_i2c_init(void);
uint8_t lidar_wait(void);
static void __lidar_receive_package(void);
static unsigned int __lidar_calculate_checksum(void);
lidar_package lidar_get_package(void);
void lidar_send_package(lidar_package value);
lidar_state lidar_get_state(void);

#endif /* PACKAGE_H */