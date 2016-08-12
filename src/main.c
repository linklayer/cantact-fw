//
// CANable firmware - a fork of CANtact by Eric Evenchick
//


#include "stm32f0xx_hal.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "can.h"
#include "slcan.h"
#include "system.h"
#include "led.h"


int main(void)
{
    HAL_Init();
    system_init();
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

    CanRxMsgTypeDef rx_msg;
    uint8_t msg_buf[SLCAN_MTU];

    while(1)
    {
		if(is_can_msg_pending(CAN_FIFO0) && can_rx(&rx_msg, 2) == HAL_OK)
		{
			uint32_t numbytes = slcan_parse_frame(msg_buf, &rx_msg);
			CDC_Transmit_FS(msg_buf, numbytes);
		}

		led_process();
    }
}

