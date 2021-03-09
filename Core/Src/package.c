#include "package.h"

package get_package(UART_HandleTypeDef *uart) {
	package value;

	// Store data in the package
	HAL_UART_Receive(uart, &value.address, 1, 100);
	HAL_UART_Receive(uart, &value.command, 1, 100);
	HAL_UART_Receive(uart, &value.hash, 4, 100);
	HAL_UART_Receive(uart, &value.size, 1, 100);
	value.data = (uint8_t*) malloc(value.size * sizeof(uint8_t));
	HAL_UART_Receive(uart, value.data, value.size, 100);
	
	return value;
}

void send_package(UART_HandleTypeDef* uart, package value) {
	HAL_UART_Transmit(uart, &value.address, 1, 1000);
	HAL_UART_Transmit(uart, &value.command, 1, 1000);
	HAL_UART_Transmit(uart, &value.hash, 4, 1000);
	HAL_UART_Transmit(uart, &value.size, 1, 1000);
	HAL_UART_Transmit(uart, value.data, value.size, 1000);
}