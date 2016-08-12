//
// CANable firmware - a fork of CANtact by Eric Evenchick
//


#include "stm32f0xx_hal.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "can.h"
#include "slcan.h"
#include "led.h"


void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void led_init(void);


volatile int i=0;
int main(void)
{

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    can_init();
    led_init();
    MX_USB_DEVICE_Init();

    // turn on green LED
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

    // blink red LED for test
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
	HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);

    // loop forever
    CanRxMsgTypeDef rx_msg;
    uint32_t status;
    uint8_t msg_buf[SLCAN_MTU];


    for (;;) {
		while (!is_can_msg_pending(CAN_FIFO0))
			led_process();
		status = can_rx(&rx_msg, 3);
		if (status == HAL_OK) {
			status = slcan_parse_frame((uint8_t *)&msg_buf, &rx_msg);
			CDC_Transmit_FS(msg_buf, status);
		}
		led_process();
    }

}

void SystemClock_Config(void)
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

}

/** Configure pins as
 * Analog
 * Input
 * Output
 * EVENT_OUT
 * EXTI
 */
void MX_GPIO_Init(void)
{

    /* GPIO Ports Clock Enable */
    __GPIOF_CLK_ENABLE();
    __GPIOA_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();
}

/* USER CODE BEGIN 4 */
static void led_init()
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;
    GPIO_InitStruct.Alternate = 0;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
