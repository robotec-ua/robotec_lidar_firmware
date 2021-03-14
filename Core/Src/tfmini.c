#include "tfmini.h"

static UART_HandleTypeDef __tfmini_uart;
static tfmini_state __tfmini_sensor_state;
static tfmini_package __tfmini_data;

/**
 * Initialize the sensor and the data
 */
void tfmini_init(void)
{
	/* FIXME : define the constants */
	__tfmini_uart.Instance = USART2;
	__tfmini_uart.Init.BaudRate = 115200;
	__tfmini_uart.Init.WordLength = UART_WORDLENGTH_8B;
	__tfmini_uart.Init.StopBits = UART_STOPBITS_1;
	__tfmini_uart.Init.Parity = UART_PARITY_NONE;
	__tfmini_uart.Init.Mode = UART_MODE_TX_RX;
	__tfmini_uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	__tfmini_uart.Init.OverSampling = UART_OVERSAMPLING_16;
	
	/* Handle the errors in configuration */
	if (HAL_UART_Init(&__tfmini_uart) != HAL_OK)
	{
		error_handler();
	}
}

/**
 * Calculating the TFMini checksum
 * @return TFMini-specific checksum
 */
uint8_t __tfmini_calculate_checksum(void) 
{
	uint64_t temp = 0;		/* Temporary container for the sum */

	/* Calculate the checksum */
	/*
	for (uint8_t index = 0; index < 8; ++index)
	{
		temp += (data >> index * 8) & 0xFF; 
	}
	*/

	/* Return only the lower bits of the checksum */
	return ((temp >> 8) & 0xFF);
}

/**
 * Waiting for the package from the sensor
 * @return 0 if there was no errors, -1 otherwise
 */
int8_t tfmini_wait(void)
{
	/* FIXME : use the timeout */
	while (1) {
		if (__HAL_UART_GET_FLAG(&__tfmini_uart, UART_FLAG_RXNE) == SET) {
			__tfmini_receive_package();

			/* Check if the package is correct by checking the CRC */
			if (__tfmini_data.checksum != __tfmini_calculate_checksum())
			{
				break;
			}

			/* Return 0 as there was no errors so far */
			return 0;
		}
	}

	/* Call the error handler */

	/* Indicating an error */
	return -1;
}

void __tfmini_receive_package(void) 
{
	// Store data in the package
	HAL_UART_Receive(&__tfmini_uart, &__tfmini_data.distance, 2, 100);
	HAL_UART_Receive(&__tfmini_uart, &__tfmini_data.strength, 2, 100);
	HAL_UART_Receive(&__tfmini_uart, &__tfmini_data.temperature, 2, 100);
}

tfmini_package tfmini_get_package(void)
{
	return __tfmini_data;
}

tfmini_state tfmini_get_state(void) 
{
	return __tfmini_sensor_state;
}

void tfmini_set_state(tfmini_state state) 
{
	__tfmini_sensor_state = state;
}

void tfmini_reset(void)
{
	unsigned int data = TFMINI_RESET_PAYLOAD;
	HAL_UART_Transmit(&__tfmini_uart, &data, 1, 1000);
}