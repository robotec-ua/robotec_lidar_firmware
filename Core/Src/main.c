/**
	******************************************************************************
	* @file					 : main.c
	* @brief					: Main program body
	******************************************************************************
	* @attention
	*
	* <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
	* All rights reserved.</center></h2>
	*
	* This software component is licensed by ST under BSD 3-Clause license,
	* the "License"; You may not use this file except in compliance with the
	* License. You may obtain a copy of the License at:
	*												opensource.org/licenses/BSD-3-Clause
	*
	******************************************************************************
	*/

#include "main.h"

CRC_HandleTypeDef hcrc;

static uint8_t __address = 0x01;

void sysclock_conf(void);
static void gpio_init(void);
static void crc_init(void);
static void halt(void);
static void reset(void);

/**
 * 
 */
int main(void)
{
	uint8_t result;							// Package waiting result
	lidar_package package;					// Device package 
	lidar_state device_state;				// Device state
	tfmini_state sensor_state;				// Sensor state
	tfmini_package sensor_package;			// Sensor data package

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	sysclock_conf();

	/* Initialize all configured peripherals */
	gpio_init();
	crc_init();
	lidar_init();
	tfmini_init();

	while (1)
	{
		/* Wait for the command to start executing */
		result = lidar_wait();
		if (result == -1) 
		{
			/* Send error to the sender */
			package.size = 1;
			package.data = RSP_CMD_ERR;
			lidar_send_package(package);
		}
		package = lidar_get_package();

		/* Check if the address is correct */
		if (__address != package.address)
		{
			/* Don't do anything */
			continue;
		}

		/* Decode the command */
		switch (package.command) {
			case CMD_STATUS_CHECK:
				device_state = lidar_get_state();
				package.size = 1;
				package.data = device_state.status;

				break;

			case CMD_SENSOR_STATUS_CHECK:
				sensor_state = tfmini_get_state();
				package.size = 1;
				package.data = sensor_state.status;

				break;

			case CMD_HALT:
				/* Stop the execution */
				halt();

				/* Create a  response */
				package.size = 1;
				package.data = RSP_CMD_DONE;

				break;

			case CMD_RESET:
				/* Stop the execution */
				reset();

				/* Create a  response */
				package.size = 1;
				package.data = RSP_CMD_DONE;

				break;

			case CMD_GET_SAMPLE_RATE:
				sensor_state = tfmini_get_state();
				package.size = 2;
				package.data = sensor_state.sample_rate;

				break;

			case CMD_SET_SAMPLE_RATE:
				/* Update the parameter */
				sensor_state = tfmini_get_state();
				sensor_state.sample_rate = package.data;
				tfmini_set_state(sensor_state);

				/* Set the response */
				package.size = 1;
				package.data = RSP_CMD_DONE;

				break;

			case CMD_GET_MAX_DIST:
				sensor_state = tfmini_get_state();
				package.size = 2;
				package.data = sensor_state.max_distance;

				break;

			case CMD_SET_MAX_DIST:
				/* Update the parameter */
				sensor_state = tfmini_get_state();
				sensor_state.max_distance = package.data;
				tfmini_set_state(sensor_state);

				/* Set the response */
				package.size = 1;
				package.data = RSP_CMD_DONE;

				break;

			case CMD_GET_ROT_ANGLE:
				break;

			case CMD_SET_ROT_ANGLE:
				break;

			case CMD_GET_ADDR:
				package.size = 1;
				package.data = (uint64_t) __address;

				break;

			case CMD_SET_ADDR:
				/* Set the new address*/
				__address = (uint8_t) package.address;

				/* Set response package data */
				package.size = 1;
				package.data = RSP_CMD_DONE;

				break;

			case CMD_SENSOR_GET_READING:
				/* Get data from the sensor */
				result = tfmini_wait();
				if (result == -1) 
				{
					/* Send error to the sender */
					package.size = 1;
					package.data = RSP_CMD_ERR;

					break;
				}
				sensor_package = tfmini_get_package();

				/* Create a response  with data*/
				package.size = 4;
				package.data = (sensor_package.distance << 16) | sensor_package.strength;

				break;

			case CMD_SENSOR_GET_TEMP:
				/* Get data from the sensor */
				result = tfmini_wait();
				if (result == -1) 
				{
					/* Send error to the sender */
					package.size = 1;
					package.data = RSP_CMD_ERR;

					break;
				}
				sensor_package = tfmini_get_package();

				/* Create a response  with data*/
				package.size = 2;
				package.data = sensor_package.temperature;

				break;

			/* Send an error if the command was not recognized */
			default :
				package.size = 1;
				package.data = RSP_CMD_ERR;

				break;
		}

		/* Send the package to the main devjce */
		lidar_send_package(package);
	}
}

/**
 * 
 */
void sysclock_conf(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Initializes the RCC Oscillators according to the specified parameters
	* in the RCC_OscInitTypeDef structure.
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		error_handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
															|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		error_handler();
	}
}

/**
 * 
 */
static void crc_init(void)
{
	hcrc.Instance = CRC;
	if (HAL_CRC_Init(&hcrc) != HAL_OK)
	{
		error_handler();
	}
}

/* FIXME : move to lidar.h */
/**
 * 
 */
/*
static void MX_I2C1_Init(void)
{
	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 100000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK)
	{
		Error_Handler();
	}
}
*/

/* FIXME : move to lidar.h */
/**
 * 
 */
/*
static void MX_SPI1_Init(void)
{
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi1) != HAL_OK)
	{
		Error_Handler();
	}
}
*/

/**
 * 
 */
static void gpio_init(void)
{
	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
}

static void halt(void)
{
	/* Stop the motor */

	/* Change the state of the device */

}

static void reset(void)
{
	/* Stop the device */
	halt();

	/* Set settings to default */
	tfmini_reset();
}

/**
 * 
 */
void error_handler(void)
{
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
}

#ifdef	USE_FULL_ASSERT
/**
 * @brief	Reports the name of the source file and the source line number
 *				 where the assert_param error has occurred.
 * @param	file: pointer to the source file name
 * @param	line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
		 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif /* USE_FULL_ASSERT */