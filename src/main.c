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
    usb_init();

    led_green_on();
    led_blue_blink(3);

    while(1)
    {
		led_process();
        can_process();
        usb_process();
    }
}

