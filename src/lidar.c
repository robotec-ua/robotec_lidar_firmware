#include "lidar.h"

static UART_HandleTypeDef 	__lidar_uart;
static I2C_HandleTypeDef 	__lidar_i2c;
static SPI_HandleTypeDef 	__lidar_spi;
static lidar_package 		__lidar_data;
static lidar_state			__lidar_state;

/**
 * Setup UART interface
 */
void lidar_uart_init(void)
{
	__lidar_uart.Instance = USART1;
	__lidar_uart.Init.BaudRate = 115200;
	__lidar_uart.Init.WordLength = UART_WORDLENGTH_8B;
	__lidar_uart.Init.StopBits = UART_STOPBITS_1;
	__lidar_uart.Init.Parity = UART_PARITY_NONE;
	__lidar_uart.Init.Mode = UART_MODE_TX_RX;
	__lidar_uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	__lidar_uart.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&__lidar_uart) != HAL_OK)
	{
		error_handler();
	}
}

/**
 * Waiting for the package from the master device
 * @return 0 if there was no errors, -1 otherwise
 */
uint8_t lidar_wait(void) {
	/* Set timer to create timeout */
	TIM1->PSC = 0;
	TIM1->ARR = 0xFFFF;
	TIM1->CR1 |= TIM_CR1_CEN;
	TIM1->CNT = 0u;

	while (TIM1->CNT < 8000u)
	{
		if (__HAL_UART_GET_FLAG(&__lidar_uart, UART_FLAG_RXNE) == SET)
		{
			/* Receive a new package */
			__lidar_receive_package();

			/* Check if the package is correct by checking the CRC */
			if (__lidar_data.hash != __lidar_calculate_checksum())
			{
				break;
			}

			/* Return 0 as there was no errors so far */
			return 0;
		}
	}

	/* Call the error handler */
	error_handler();

	/* Indicating an error */
	return -1;
}

/**
 * Receive packaged data from the master device
 */
static void __lidar_receive_package(void) 
{
	// Store data in the package
	HAL_UART_Receive(&__lidar_uart, &__lidar_data.header, 1, 100);
	HAL_UART_Receive(&__lidar_uart, &__lidar_data.address, 1, 100);
	HAL_UART_Receive(&__lidar_uart, &__lidar_data.command, 1, 100);
	HAL_UART_Receive(&__lidar_uart, &__lidar_data.hash, 1, 100);
	HAL_UART_Receive(&__lidar_uart, &__lidar_data.size, 1, 100);
	HAL_UART_Receive(&__lidar_uart, &__lidar_data.data, __lidar_data.size, 100);
}

/**
 * Get current package
 * @return the last received package
 */
lidar_package lidar_get_package(void) 
{
	// FIXME : clear the received package after fetching
	return __lidar_data;
}

/**
 * Send data to the master device
 * @param value the package to send
 */
void lidar_send_package(lidar_package value)
{
	HAL_UART_Transmit(&__lidar_uart, &value.header, 1, 1000);
	HAL_UART_Transmit(&__lidar_uart, &value.address, 1, 1000);
	HAL_UART_Transmit(&__lidar_uart, &value.command, 1, 1000);
	HAL_UART_Transmit(&__lidar_uart, &value.hash, 4, 1000);
	HAL_UART_Transmit(&__lidar_uart, &value.size, 1, 1000);
	HAL_UART_Transmit(&__lidar_uart, &value.data, value.size, 1000);
}

/**
 * Calculate the checksum
 * @return checksum of the current package
 */
static unsigned int __lidar_calculate_checksum(void) 
{
	unsigned int checksum;

	return checksum;
}

/**
 * Get device's status and settings
 * @return the struct with data about the device
 */
lidar_state lidar_get_state(void) 
{
	return __lidar_state;
}

/**
 * SPI initialization
 */
void lidar_spi_init(void)
{
	__lidar_spi.Instance = SPI1;
	__lidar_spi.Init.Mode = SPI_MODE_MASTER;
	__lidar_spi.Init.Direction = SPI_DIRECTION_2LINES;
	__lidar_spi.Init.DataSize = SPI_DATASIZE_8BIT;
	__lidar_spi.Init.CLKPolarity = SPI_POLARITY_LOW;
	__lidar_spi.Init.CLKPhase = SPI_PHASE_1EDGE;
	__lidar_spi.Init.NSS = SPI_NSS_SOFT;
	__lidar_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	__lidar_spi.Init.FirstBit = SPI_FIRSTBIT_MSB;
	__lidar_spi.Init.TIMode = SPI_TIMODE_DISABLE;
	__lidar_spi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	__lidar_spi.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&__lidar_spi) != HAL_OK)
	{
		error_handler();
	}
}

/**
 * 12C initialization
 */
void lidar_i2c_init(void)
{
	__lidar_i2c.Instance = I2C1;
	__lidar_i2c.Init.ClockSpeed = 100000;
	__lidar_i2c.Init.DutyCycle = I2C_DUTYCYCLE_2;
	__lidar_i2c.Init.OwnAddress1 = 0;
	__lidar_i2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	__lidar_i2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	__lidar_i2c.Init.OwnAddress2 = 0;
	__lidar_i2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	__lidar_i2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&__lidar_i2c) != HAL_OK)
	{
		error_handler();
	}
}