#ifndef PACKAGE_H
#define PACKAGE_H

#include "stm32f1xx_hal.h"
#include <stdlib.h>
#include <stdint.h>

/**
 * Struct which represents the package of transmitted data
 */
typedef struct {
	uint8_t address;		// Adress of the device
	uint8_t command;		// Command to the device
	uint32_t hash;			// Hash of the package (CRC32)
	uint8_t size;			// Size of the package
	uint8_t* data;			// Additional data needed by the devices
} package ;

package get_package(UART_HandleTypeDef *uart);
void send_package(UART_HandleTypeDef *uart, package value);

#endif /* PACKAGE_H */