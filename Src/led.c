//
// LED: Handles blinking of status light
//

#include "stm32f0xx_hal.h"
#include "led.h"

static uint32_t led_laston = 0;
static uint32_t led_lastoff = 0;

// Attempt to turn on status LED
void led_on(void)
{
	// Make sure the LED has been off for at least LED_DURATION before turning on again
	// This prevents a solid status LED on a busy canbus
	if(led_laston == 0 && HAL_GetTick() - led_lastoff > LED_DURATION)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);
		led_laston = HAL_GetTick();
	}
}


// Process time-based LED events
void led_process(void)
{
	// If LED has been on for long enough, turn it off
	if(led_laston > 0 && HAL_GetTick() - led_laston > LED_DURATION)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0);
		led_laston = 0;
		led_lastoff = HAL_GetTick();
	}
}

