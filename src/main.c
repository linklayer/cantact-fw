//
// CANable firmware - a fork of CANtact by Eric Evenchick
//


#include "stm32f0xx.h"
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
    system_init();
    can_init();
    led_init();
    usb_init();

    led_blue_blink(2);

    // Storage for status and received message buffer
    CAN_RxHeaderTypeDef rx_msg_header;
    uint8_t rx_msg_data[8] = {0};
    uint8_t msg_buf[SLCAN_MTU];


    while(1)
    {
        // Block until a CAN message is received
        while (!is_can_msg_pending(CAN_RX_FIFO0))
            led_process();

        uint32_t status = can_rx(&rx_msg_header, rx_msg_data);

        // If message received from bus, parse the frame
        if (status == HAL_OK)
        {
        	uint16_t msg_len = slcan_parse_frame((uint8_t *)&msg_buf, &rx_msg_header, rx_msg_data);

            // Transmit message via USB-CDC 
            if(msg_len)
            {
                CDC_Transmit_FS(msg_buf, msg_len);
            }
        }

        led_process();
    }
}

