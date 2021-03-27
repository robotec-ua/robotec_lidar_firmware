#ifndef MOTOR_H
#define MOTOR_H

/* Include C headers*/
#include <stdint.h>

/* Include STMicroelectronics headers */
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_tim.h"

/* Define constants */
#define MOTOR_PORT		GPIOD
#define MOTOR_PIN		GPIO_PinSource12
#define MOTOR_PRESC		40000
#define MOTOR_PERIOD	500

/**
 * Struct with PWM parameters
 */
typedef struct {
	uint32_t clock;
	uint32_t period;
	uint32_t pulse_period;
} motor_pwm_param;

uint16_t motor_get_rotation_angle(void);
void motor_set_rotation_angle(uint16_t value);
void motor_init(void);
void motor_set_duty_cycle(uint8_t duty_cycle);
static void __motor_timer_init(void);
static void __motor_pwm_init(void);

#endif /* MOTOR_H */