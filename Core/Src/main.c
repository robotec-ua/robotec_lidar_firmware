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
I2C_HandleTypeDef hi2c1;
SPI_HandleTypeDef hspi1;
UART_HandleTypeDef uart_out;

unsigned int address = 0x01;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CRC_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);

/*Device section*/
uint8_t send_status(void);
void halt(void);
uint16_t get_address(void);
void set_address(uint16_t value);

/*Motor section*/
/*FIXME : move to motor.h*/
uint16_t get_rotation_angle(void);
void set_rotation_angle(uint8_t value);

/**
 * @brief The application entry point.
 * @retval int
 */
int main(void)
{
	package received_package;
	unsigned long lidar_package;
	uint8_t command = 0;

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_CRC_Init();
	MX_I2C1_Init();
	MX_SPI1_Init();
	MX_USART1_UART_Init();
	tfmini_init();

	while (1)
	{
		/* FIXME : move to the function named "package_wait" */
		/* If there is a transfer on the line, receive the command */
		if (__HAL_UART_GET_FLAG(&uart_out, UART_FLAG_RXNE) == SET)
		{
			received_package = get_package(&uart_out);
		}

		/* Check if the address is correct */
		if (address != received_package.address)
		{

		}

		/* Check if the CRC is correct */

		/* Decode the command */
		switch (received_package.command) {
			case CMD_STATUS_CHECK:
				/*send_status();*/
				break;

			case CMD_SENSOR_STATUS_CHECK:
				/*send_sensor_status();*/
				break;

			case CMD_HALT:
				/*halt();*/
				break;

			case CMD_RESET:
				/*sensor_reset();*/
				break;

			case CMD_GET_SAMPLE_RATE:
				/*get_sample_rate();*/
				break;

			case CMD_SET_SAMPLE_RATE:
				/*set_sample_rate();*/
				break;

			case CMD_SET_MODE:
				/*set_mode();*/
				break;

			case CMD_GET_MAX_DIST:
				/*get_max_distance();*/
				break;

			case CMD_SET_MAX_DIST:
				/*set_max_distance();*/

			case CMD_GET_ROT_ANGLE:
				/*get_rotation_angle();*/
				break;

			case CMD_SET_ROT_ANGLE:
				/*set_rotation_angle();*/
				break;

			case CMD_GET_ADDR:
				/*get_address();*/
				break;

			case CMD_SET_ADDR:
				/*set_address();*/
				break;

			default :
				/* TODO */
				break;
		}

		/*Reset the command*/
		command = 0;
	}
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
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
		Error_Handler();
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
		Error_Handler();
	}
}

/**
 * @brief CRC Initialization Function
 * @param None
 * @retval None
 */
static void MX_CRC_Init(void)
{
	hcrc.Instance = CRC;
	if (HAL_CRC_Init(&hcrc) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
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

/**
 * @brief SPI1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI1_Init(void)
{
	/* SPI1 parameter configuration*/
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

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void)
{
	uart_out.Instance = USART1;
	uart_out.Init.BaudRate = 115200;
	uart_out.Init.WordLength = UART_WORDLENGTH_8B;
	uart_out.Init.StopBits = UART_STOPBITS_1;
	uart_out.Init.Parity = UART_PARITY_NONE;
	uart_out.Init.Mode = UART_MODE_TX_RX;
	uart_out.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	uart_out.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&uart_out) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
}

/**
 * @brief	This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
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