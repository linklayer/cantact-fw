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
    uint32_t status;
    uint8_t msg_buf[SLCAN_MTU];

    // loop forever
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

