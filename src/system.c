//
// system: initalize system clocks and other core peripherals
//

#include "stm32f0xx_hal.h"
#include "system.h"


// Initialize system clocks
void system_init(void)
{
    HAL_Init();


    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInit;

    // Set up the oscillators
    // use internal HSI48 (48 MHz), no PLL
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
    RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;

    // Set sysclk, hclk, and pclk1 source to HSI48 (48 MHz)
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK |
				   RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

    // Set USB clock source to HSI48 (48 MHz)
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
    PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;

    HAL_RCC_OscConfig(&RCC_OscInitStruct);
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

    // Enable clock recovery system for internal oscillator
    RCC_CRSInitTypeDef RCC_CRSInitStruct;
    __HAL_RCC_CRS_CLK_ENABLE();
	
    // Default Synchro Signal division factor (not divided) 
    RCC_CRSInitStruct.Prescaler = RCC_CRS_SYNC_DIV1;

    // Set the SYNCSRC[1:0] bits according to CRS_Source value 
    RCC_CRSInitStruct.Source = RCC_CRS_SYNC_SOURCE_USB;

    // Rising polarity
    RCC_CRSInitStruct.Polarity = RCC_CRS_SYNC_POLARITY_RISING;

    // HSI48 is synchronized with USB SOF at 1KHz rate 
    RCC_CRSInitStruct.ReloadValue = __HAL_RCC_CRS_RELOADVALUE_CALCULATE(48000000, 1000);
    RCC_CRSInitStruct.ErrorLimitValue = RCC_CRS_ERRORLIMIT_DEFAULT;

    // Set the TRIM[5:0] to the default value
    RCC_CRSInitStruct.HSI48CalibrationValue = 32;

    // Start automatic synchronization 
    HAL_RCCEx_CRSConfig(&RCC_CRSInitStruct);
	
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
}


// Convert a 32-bit value to an ascii hex value
void system_hex32(char *out, uint32_t val)
{
	char *p = out + 8;
	*p-- = 0;
	while (p >= out) {
		uint8_t nybble = val & 0x0F;
		if (nybble < 10)
			*p = '0' + nybble;
		else
			*p = 'A' + nybble - 10;
		val >>= 4;
		p--;
	}
} 


// Disable all interrupts
void system_irq_disable(void)
{
        __disable_irq();
        __DSB();
        __ISB();
}


// Enable all interrupts
void system_irq_enable(void)
{
        __enable_irq();
}

