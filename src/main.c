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

CRC_HandleTypeDef __crc;

static uint8_t __address = 0x01;

void sysclock_conf(void);
static void gpio_init(void);
static void crc_init(void);
static void halt(void);
static void reset(void);

/**
 * STM32 software entry point
 * @return exit code
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
	motor_init();
	gpio_init();
	crc_init();
	tfmini_init();

	// FIXME : check the parameters and use the appropriate protocol  
	lidar_uart_init();

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
 * Configure clock parameters of the internal clock source
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
 * Initialize internal CRC module
 */
static void crc_init(void)
{
	__crc.Instance = CRC;
	if (HAL_CRC_Init(&__crc) != HAL_OK)
	{
		error_handler();
	}
}

/**
 * Initialize chip's GPIO
 */
static void gpio_init(void)
{
	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
}

/**
 * Halt execution of current tasks
 */
static void halt(void)
{
	/* Stop the motor */

	/* Change the state of the device */

}

/**
 * Reset the device to the original state
 */
static void reset(void)
{
	/* Stop the device */
	halt();

	/* Set settings to default */
	tfmini_reset();
}

/**
 * Global error handler
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