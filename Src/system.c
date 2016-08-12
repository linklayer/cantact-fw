//
// system: initalize system clocks and other core peripherals
//

#include "stm32f0xx_hal.h"
#include "system.h"


// Initialize system clocks
void system_init(void)
{

    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInit;

#ifdef INTERNAL_OSCILLATOR
    // set up the oscillators
    // use internal HSI48 (48 MHz), no PLL
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
    RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;

    // set sysclk, hclk, and pclk1 source to HSI48 (48 MHz)
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK |
				   RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

    // set USB clock source to HSI48 (48 MHz)
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
    PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;


#else
    // set up the oscillators
    // use external oscillator (16 MHz), enable 3x PLL (48 MHz)
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL3;
    RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;

    // set sysclk, hclk, and pclk1 source to PLL (48 MHz)
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK |
				   RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

    // set USB clock source to PLL (48 MHz)
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
    PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLLCLK;

#endif

    HAL_RCC_OscConfig(&RCC_OscInitStruct);
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
    __SYSCFG_CLK_ENABLE();


	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);


#ifdef INTERNAL_OSCILLATOR
    // Enable clock recovery system for internal oscillator
    RCC_CRSInitTypeDef RCC_CRSInitStruct;

    // Enable CRS Clock
    __CRS_CLK_ENABLE();

    // Default Synchro Signal division factor (not divided) 
    RCC_CRSInitStruct.Prescaler = RCC_CRS_SYNC_DIV1;

    // Set the SYNCSRC[1:0] bits according to CRS_Source value 
    RCC_CRSInitStruct.Source = RCC_CRS_SYNC_SOURCE_USB;

    // HSI48 is synchronized with USB SOF at 1KHz rate 
    RCC_CRSInitStruct.ReloadValue = __HAL_RCC_CRS_CALCULATE_RELOADVALUE(48000000, 1000);
    RCC_CRSInitStruct.ErrorLimitValue = RCC_CRS_ERRORLIMIT_DEFAULT;

    // Set the TRIM[5:0] to the default value
    RCC_CRSInitStruct.HSI48CalibrationValue = 0x20; 

    // Start automatic synchronization 
    HAL_RCCEx_CRSConfig(&RCC_CRSInitStruct);

    // Force sync event
    HAL_RCCEx_CRSSoftwareSynchronizationGenerate();

    // Wait until synchronized
    HAL_RCCEx_CRSWaitSynchronization(3000);
#endif

    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
    __GPIOF_CLK_ENABLE();
    __GPIOA_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();
}

