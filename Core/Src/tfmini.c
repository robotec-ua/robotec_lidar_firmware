#include "tfmini.h"

static UART_HandleTypeDef tfmini_uart;
static struct __tfmini_state __sensor_state;
static struct __tfmini_package __tfmini_data;

/**
 * Initialize the sensor and the data
 */
void tfmini_init(void)
{
	/* FIXME : define the constants */
	tfmini_uart.Instance = USART2;
	tfmini_uart.Init.BaudRate = 115200;
	tfmini_uart.Init.WordLength = UART_WORDLENGTH_8B;
	tfmini_uart.Init.StopBits = UART_STOPBITS_1;
	tfmini_uart.Init.Parity = UART_PARITY_NONE;
	tfmini_uart.Init.Mode = UART_MODE_TX_RX;
	tfmini_uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	tfmini_uart.Init.OverSampling = UART_OVERSAMPLING_16;
	
	/* Handle the errors in configuration */
	if (HAL_UART_Init(&tfmini_uart) != HAL_OK)
	{
		__tfmini_error_handler();
	}
}

/**
 * Calculating the TFMini checksum
 * @param data the data to calculate the checksum from
 * @return TFMini-specific checksum
 */
uint8_t __tfmini_calculate_checksum(uint64_t data) 
{
	uint64_t temp = 0;		/* Temporary container for the sum */

	/* Calculate the checksum */
	for (uint8_t index = 0; index < 8; ++index)
	{
		temp += (data >> index * 8) & 0xFF; 
	}

	/* Return only the lower bits of the checksum */
	return ((temp >> 8) & 0xFF);
}

/**
 * Unpack the data into the internal package to store
 * @param data the data from the sensor to unpack
 */
void __tfmini_unpack(uint64_t data) {
	// Fill the package if the checksum is correct
	__tfmini_data.distance = TFMINI_GET_DIST(data);
	__tfmini_data.strength = TFMINI_GET_STRENGTH(data);
	__tfmini_data.temperature = TFMINI_GET_TEMP(data);
}

/**
 * Handling the errors of the sensor
 */
void __tfmini_error_handler(void)
{
	__disable_irq();

	/* Set the status to ERROR and send the error package to the master device */

	while (1)
	{}
}

/**
 * Waiting for the package from the sensor
 * @return 0 if there was no errors, -1 otherwise
 */
int8_t tfmini_wait(void)
{
	uint64_t data;
	uint8_t checksum;

	/* FIXME : use the timeout */
	while (1) {
		if (__HAL_UART_GET_FLAG(&tfmini_uart, UART_FLAG_RXNE) == SET) {
			/* Get data if the timeout is not reached */
			HAL_UART_Receive(&tfmini_uart, &data, 8, 100);
			HAL_UART_Receive(&tfmini_uart, &checksum, 1, 100);

			/* Check if the package is correct by checking the CRC */
			checksum = TFMINI_GET_CHECKSUM(data);
			if (checksum != __tfmini_calculate_checksum(data))
			{
				break;
			}

			/* Save data to the internal structure */
			__tfmini_unpack(data);

			/* Return 0 as there was no errors so far */
			return 0;
		}
	}

	/* Call the error handler */

	/* Indicating an error */
	return -1;
}

/**
 * Getting the current status of the sensor
 * @return the status of the sensor
 */
uint8_t tfmini_get_sensor_status(void)
{
	return __sensor_state.status;
}

/**
 * Reset the sensor parameters
 */
void tfmini_sensor_reset(void)
{

}

/**
 * Getting the sample rate of the sensor
 * @return the sample rate of the sensor
 */
uint16_t tfmini_get_sample_rate(void) {
	return __sensor_state.sample_rate;
}

/**
 * Setting the sample rate of the sensor
 * @param value the sample rate of the sensor
 */
void tfmini_set_sample_rate(uint16_t value) 
{
	/* Send the corresponding command to the device */
	/* FIXME */

	/* Update internal struct data */
	 __sensor_state.sample_rate = value;
}

/**
 * Setting the mode of the sensor
 * @param value the mode
 */
void tfmini_set_mode(uint8_t value)
{
	/* Send the corresponding command to the device */
	/* FIXME */

	/* Update internal struct data */
	 __sensor_state.mode = value;
}

/**
 * Getting the mode of the sensor
 * @return the mode
 */
uint8_t tfmini_get_mode(void)
{
	return __sensor_state.mode;
}

/**
 * Getting the maximum distance of the sensor
 * @return the max distance
 */
uint8_t tfmini_get_max_distance(void)
{
	return __sensor_state.max_distance;
}

/**
 * Setting the maximum distance of the sensor
 * @param value the max distance
 */
void tfmini_set_max_distance(uint16_t value)
{
	/* Send the corresponding command to the device */
	/* FIXME */

	/* Update internal struct data */
	 __sensor_state.max_distance = value;
}