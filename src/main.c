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
    // Initialize peripherals
    HAL_Init();
    system_init();
    can_init();
    led_init();
    usb_init();

    led_blue_blink(2);

    // Storage for status and recieved message buffer
    CanRxMsgTypeDef rx_msg;
    uint32_t status;
    uint8_t msg_buf[SLCAN_MTU];
    uint16_t msg_len = 0;


    while(1)
    {
        // Block until a CAN message is recieved
        while (!is_can_msg_pending(CAN_FIFO0))
            led_process();

        status = can_rx(&rx_msg, 3);

        // If message received from bus, parse the frame
        if (status == HAL_OK)
        {
            msg_len = slcan_parse_frame((uint8_t *)&msg_buf, &rx_msg);

            // Transmit message via USB-CDC 
            if(msg_len)
            {
                CDC_Transmit_FS(msg_buf, msg_len);
            }
        }

        led_process();
    }
}

