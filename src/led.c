//
// LED: Handles blinking of status light
//

#include "stm32f0xx_hal.h"
#include "led.h"

static volatile uint32_t led_laston = 0;
static uint32_t led_lastoff = 0;


// Initialize LED GPIOs
void led_init()
{
    __GPIOB_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;
    GPIO_InitStruct.Alternate = 0;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}


// Turn green LED on
void led_green_on(void)
{
	HAL_GPIO_WritePin(LED_GREEN, 1);
}


// Turn green LED on
void led_green_off(void)
{
	HAL_GPIO_WritePin(LED_GREEN, 0);
}


// Blink blue LED (blocking)
void led_blue_blink(uint8_t numblinks)
{
	uint8_t i;
	for(i=0; i<numblinks; i++)
	{
		HAL_GPIO_WritePin(LED_BLUE, 1);
		HAL_Delay(100);
		HAL_GPIO_WritePin(LED_BLUE, 0);
		HAL_Delay(100);
	}
}

// Attempt to turn on status LED
void led_blue_on(void)
{
	// Make sure the LED has been off for at least LED_DURATION before turning on again
	// This prevents a solid status LED on a busy canbus
	if(led_laston == 0 && HAL_GetTick() - led_lastoff > LED_DURATION)
	{
		HAL_GPIO_WritePin(LED_BLUE, 1);
		led_laston = HAL_GetTick();
	}
}


// Process time-based LED events
void led_process(void)
{
	// If LED has been on for long enough, turn it off
	if(led_laston > 0 && HAL_GetTick() - led_laston > LED_DURATION)
	{
		HAL_GPIO_WritePin(LED_BLUE, 0);
		led_laston = 0;
		led_lastoff = HAL_GetTick();
	}
}

