#include "motor.h"

static TIM_HandleTypeDef 	__motor_pwm;
static TIM_OC_InitTypeDef 	__motor_pwm_channel;

/**
 * Initialize the motor
 */
void motor_init(void) 
{
	__motor_timer_init();
	__motor_pwm_init();
}

/**
 * Timer initialization
 */
void __motor_timer_init(void)
{
	/* Enable timer clock */
	__HAL_RCC_TIM3_CLK_ENABLE();

	/* Set timer parametets */
	__motor_pwm.Instance = TIM3;
	__motor_pwm.Init.Prescaler = MOTOR_PRESC;
	__motor_pwm.Init.CounterMode = TIM_COUNTERMODE_UP;
	__motor_pwm.Init.Period = MOTOR_PERIOD;
	__motor_pwm.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	/* Initialize the timer */
	HAL_TIM_PWM_Init(&__motor_pwm);
}

/**
 * PWM initialization
 */
void __motor_pwm_init(void)
{
	__motor_pwm_channel.OCMode = TIM_OCMODE_PWM1;
	__motor_pwm_channel.Pulse = 0;
	__motor_pwm_channel.OCPolarity = TIM_OCPOLARITY_HIGH;
	__motor_pwm_channel.OCFastMode = TIM_OCFAST_DISABLE;

	/* Save the parameters */
	HAL_TIM_PWM_ConfigChannel(&__motor_pwm, &__motor_pwm_channel, TIM_CHANNEL_1);

	/* Start the PWM */
	HAL_TIM_Base_Start(&__motor_pwm); 
	HAL_TIM_PWM_Start(&__motor_pwm, TIM_CHANNEL_1); //Start Pwm signal on PB-6 Pin
}

/**
 * Set new pulse width
 * @param duty_cycle new value of duty cycle
 */
void motor_set_duty_cycle(uint8_t duty_cycle)
{
	/* Set new pulse width */
	uint8_t pulse_width = (MOTOR_PERIOD / 100) * duty_cycle;
	__HAL_TIM_SET_COMPARE(&__motor_pwm, TIM_CHANNEL_1, pulse_width);
}