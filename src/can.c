//
// can: initializes and provides methods to interact with the CAN peripheral
//

#include "stm32f0xx_hal.h"
#include "slcan.h"
#include "usbd_cdc_if.h"
#include "can.h"
#include "led.h"


// Private variables
static CAN_HandleTypeDef can_handle;
static CAN_FilterTypeDef filter;
static uint32_t prescaler;
enum can_bus_state bus_state;
static uint8_t can_nart = DISABLE;


// Initialize CAN peripheral settings, but don't actually start the peripheral
void can_init(void)
{
    // Initialize GPIO for CAN transceiver 
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_CAN1_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    //PB8     ------> CAN_RX
    //PB9     ------> CAN_TX
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_CAN;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


    // Initialize default CAN filter configuration
    filter.FilterIdHigh = 0;
    filter.FilterIdLow = 0;
    filter.FilterMaskIdHigh = 0;
    filter.FilterMaskIdLow = 0;
    filter.FilterFIFOAssignment = CAN_RX_FIFO0;
    filter.FilterBank = 0;
    filter.FilterMode = CAN_FILTERMODE_IDMASK;
    filter.FilterScale = CAN_FILTERSCALE_32BIT;
    filter.FilterActivation = ENABLE;


    // default to 125 kbit/s
    prescaler = 48;
    can_handle.Instance = CAN;
    bus_state = OFF_BUS;
}


// Start the CAN peripheral
void can_enable(void)
{
    if (bus_state == OFF_BUS)
    {
    	can_handle.Init.Prescaler = prescaler;
    	can_handle.Init.Mode = CAN_MODE_NORMAL;

    	can_handle.Init.SyncJumpWidth = CAN_SJW_1TQ;
    	can_handle.Init.TimeSeg1 = CAN_BS1_4TQ;
    	can_handle.Init.TimeSeg2 = CAN_BS2_3TQ;
    	can_handle.Init.TimeTriggeredMode = DISABLE;
    	can_handle.Init.AutoBusOff = ENABLE;
    	can_handle.Init.AutoWakeUp = DISABLE;
    	can_handle.Init.AutoRetransmission = can_nart;
    	can_handle.Init.ReceiveFifoLocked = DISABLE;
    	can_handle.Init.TransmitFifoPriority = DISABLE;
        HAL_CAN_Init(&can_handle);

        HAL_CAN_ConfigFilter(&can_handle, &filter);

        HAL_CAN_Start(&can_handle);
        bus_state = ON_BUS;

        led_blue_on();
    }
}


// Disable the CAN peripheral and go off-bus
void can_disable(void)
{
    if (bus_state == ON_BUS)
    {
        // Do a bxCAN reset (set RESET bit to 1)
    	can_handle.Instance->MCR |= CAN_MCR_RESET;
        bus_state = OFF_BUS;

        led_green_on();
    }
}


// Set the bitrate of the CAN peripheral
void can_set_bitrate(enum can_bitrate bitrate)
{
    if (bus_state == ON_BUS)
    {
        // cannot set bitrate while on bus
        return;
    }

    switch (bitrate)
    {
        case CAN_BITRATE_10K:
        prescaler = 600;
            break;
        case CAN_BITRATE_20K:
        prescaler = 300;
            break;
        case CAN_BITRATE_50K:
        prescaler = 120;
            break;
        case CAN_BITRATE_100K:
            prescaler = 60;
            break;
        case CAN_BITRATE_125K:
            prescaler = 48;
            break;
        case CAN_BITRATE_250K:
            prescaler = 24;
            break;
        case CAN_BITRATE_500K:
            prescaler = 12;
            break;
        case CAN_BITRATE_750K:
            prescaler = 8;
            break;
        case CAN_BITRATE_1000K:
            prescaler = 6;
            break;
    }

    led_green_on();
}


// Set CAN peripheral to silent mode
void can_set_silent(uint8_t silent)
{
    if (bus_state == ON_BUS)
    {
        // cannot set silent mode while on bus
        return;
    }
    if (silent)
    {
    	can_handle.Init.Mode = CAN_MODE_SILENT;
    } else {
    	can_handle.Init.Mode = CAN_MODE_NORMAL;
    }

    led_green_on();
}


// Set CAN peripheral to silent mode
void can_set_autoretransmit(uint8_t autoretransmit)
{
    if (bus_state == ON_BUS)
    {
        // Cannot set autoretransmission while on bus
        return;
    }
    if (autoretransmit)
    {
        can_nart = DISABLE;
    } else {
        can_nart = ENABLE;
    }

    led_green_on();
}


// Send a message on the CAN bus (blocking)
uint32_t can_tx(CAN_TxHeaderTypeDef *tx_msg_header, uint8_t* tx_msg_data)
{
    uint32_t status;

    // Transmit can frame
    uint32_t mailbox_txed = 0;
    status = HAL_CAN_AddTxMessage(&can_handle, tx_msg_header, tx_msg_data, &mailbox_txed);

    led_green_on();

    return status;
}


// Receive message from the CAN bus (blocking)
uint32_t can_rx(CAN_RxHeaderTypeDef *rx_msg_header, uint8_t* rx_msg_data)
{
    uint32_t status;
    status = HAL_CAN_GetRxMessage(&can_handle, CAN_RX_FIFO0, rx_msg_header, rx_msg_data);

	led_blue_on();
    return status;
}


// Check if a CAN message has been received and is waiting in the FIFO
uint8_t is_can_msg_pending(uint8_t fifo)
{
    if (bus_state == OFF_BUS)
    {
        return 0;
    }

    return(HAL_CAN_GetRxFifoFillLevel(&can_handle, CAN_RX_FIFO0) > 0);
}


// Return reference to CAN handle
CAN_HandleTypeDef* can_gethandle(void)
{
	return &can_handle;
}
