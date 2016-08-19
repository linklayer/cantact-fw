//
// can: initializes and provides methods to interact with the CAN peripheral
//

#include "stm32f0xx_hal.h"
#include "slcan.h"
#include "usbd_cdc_if.h"
#include "can.h"
#include "led.h"


static CAN_HandleTypeDef can_handle;
static CAN_FilterConfTypeDef filter;
static uint32_t prescaler;
enum can_bus_state bus_state;

static volatile uint8_t process_recv = 0;
static volatile uint8_t process_tx = 0;

static CanRxMsgTypeDef can_rx_msg;
static CanTxMsgTypeDef can_tx_msg;


// Initialize CAN peripheral settings, but don't actually start the peripheral
void can_init(void)
{
    // Initialize GPIO for CAN transceiver 
    GPIO_InitTypeDef GPIO_InitStruct;
    __CAN_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();

    //PB8     ------> CAN_RX
    //PB9     ------> CAN_TX
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF4_CAN;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


    // Initialize default CAN filter configuration
    filter.FilterIdHigh = 0;
    filter.FilterIdLow = 0;
    filter.FilterMaskIdHigh = 0;
    filter.FilterMaskIdLow = 0;
    filter.FilterMode = CAN_FILTERMODE_IDMASK;
    filter.FilterScale = CAN_FILTERSCALE_32BIT;
    filter.FilterNumber = 0;
    filter.FilterFIFOAssignment = CAN_FIFO0;
    filter.BankNumber = 0;
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

    	HAL_NVIC_SetPriority(CEC_CAN_IRQn, 0, 0);
    	HAL_NVIC_EnableIRQ(CEC_CAN_IRQn);

    	can_handle.Init.Prescaler = prescaler;
    	can_handle.Init.Mode = CAN_MODE_NORMAL;
    	can_handle.Init.SJW = CAN_SJW_1TQ;
    	can_handle.Init.BS1 = CAN_BS1_4TQ;
    	can_handle.Init.BS2 = CAN_BS2_3TQ;
    	can_handle.Init.TTCM = DISABLE;
    	can_handle.Init.ABOM = ENABLE;
    	can_handle.Init.AWUM = DISABLE;
    	can_handle.Init.NART = DISABLE;
    	can_handle.Init.RFLM = DISABLE;
    	can_handle.Init.TXFP = DISABLE;
    	can_handle.pTxMsg = &can_tx_msg;
    	can_handle.pRxMsg = &can_rx_msg;
        HAL_CAN_Init(&can_handle);
        HAL_CAN_ConfigFilter(&can_handle, &filter);
        bus_state = ON_BUS;

        led_blue_on();

    	HAL_CAN_Receive_IT(&can_handle, CAN_FIFO0);

    }
}


// Disable the CAN peripheral and go off-bus
void can_disable(void)
{
    if (bus_state == ON_BUS)
    {
        // do a bxCAN reset (set RESET bit to 1)
    	can_handle.Instance->MCR |= CAN_MCR_RESET;
        bus_state = OFF_BUS;

    	HAL_NVIC_DisableIRQ(CEC_CAN_IRQn);
    	HAL_CAN_DeInit(&can_handle);
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
}


// Send a message on the CAN bus (blocking)
uint32_t can_tx(CanTxMsgTypeDef *tx_msg)
{
    uint32_t status;

    // transmit can frame
    can_handle.pTxMsg = tx_msg;
    status = HAL_CAN_Transmit_IT(&can_handle);

	//led_blue_on();
    return status;
}


CanTxMsgTypeDef localmsg;
static volatile uint8_t can_rearm_rx = 0;


void can_process(void)
{
    if(process_recv)
    {
        CanRxMsgTypeDef* rxmsg = can_handle.pRxMsg;
        uint8_t msg_buf[SLCAN_MTU];
        uint32_t numbytes = slcan_parse_frame(msg_buf, rxmsg);
        CDC_Transmit_FS(msg_buf, numbytes);

        process_recv = 0;
    }

    if(process_tx)
    {
        uint32_t res = can_tx(&localmsg);

        // If result okay, we're done. Otherwise retry on the next time around.
        if(res == HAL_OK)
        {
            process_tx = 0;
        }

    }


    // If we were trying to transmit while starting the next rx cycle, defer to here
    if(can_rearm_rx > 0)
    {
        uint32_t res = HAL_CAN_Receive_IT(&can_handle, CAN_FIFO0);
        process_tx = 0;
        if(res != HAL_OK)
        {
            led_green_off();
            can_rearm_rx = 1;
        }
        else
        {
            led_green_on();
            can_rearm_rx = 0;
        }

    }

}

void can_preptx(CanTxMsgTypeDef *msg)
{
    localmsg = *msg;
    process_tx = 1;
}

// CAN rxcomplete callback TODO: Move to interrupts?
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
	led_blue_on();
    process_recv = 1;
	uint32_t res = HAL_CAN_Receive_IT(hcan, CAN_FIFO0);

    if(res != HAL_OK)
    {
        can_rearm_rx = 1;
    }
}
/*
void HAL_CAN_TxCpltCallback(CAN_HandleTypeDef *hcan)
{

}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
    //error_assert(ERR_CANBUS);
}
*/

// Check if a CAN message has been received and is waiting in the FIFO
uint8_t is_can_msg_pending(uint8_t fifo)
{
    if (bus_state == OFF_BUS)
    {
        return 0;
    }
    return (__HAL_CAN_MSG_PENDING(&can_handle, fifo) > 0);
}


// Return reference to CAN handle
CAN_HandleTypeDef* can_gethandle(void)
{
	return &can_handle;
}
