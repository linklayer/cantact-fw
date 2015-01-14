#include "stm32f0xx_hal.h"
#include "can.h"

CAN_HandleTypeDef hcan;
CAN_FilterConfTypeDef filter;
enum can_bus_state bus_state;

void can_init(void) {
    uint32_t status;

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

    hcan.Instance = CAN;
    hcan.Init.Prescaler = 48;
    hcan.Init.Mode = CAN_MODE_NORMAL;
    hcan.Init.SJW = CAN_SJW_1TQ;
    hcan.Init.BS1 = CAN_BS1_4TQ;
    hcan.Init.BS2 = CAN_BS2_3TQ;
    hcan.Init.TTCM = DISABLE;
    hcan.Init.ABOM = DISABLE;
    hcan.Init.AWUM = DISABLE;
    hcan.Init.NART = DISABLE;
    hcan.Init.RFLM = DISABLE;
    hcan.Init.TXFP = DISABLE;

    bus_state = OFF_BUS;
    //status = HAL_CAN_Init(&hcan);
}

void can_enable(void) {
    uint32_t status;
    if (bus_state == OFF_BUS) {
        hcan.Init.Mode = CAN_MODE_NORMAL;
        hcan.pTxMsg = NULL;
        status = HAL_CAN_Init(&hcan);
        status = HAL_CAN_ConfigFilter(&hcan, &filter);
        bus_state = ON_BUS;
    }
}

void can_disable(void) {
    uint32_t status;
    if (bus_state == ON_BUS) {
        // do a bxCAN reset (set RESET bit to 1)
        hcan.Instance->MCR |= CAN_MCR_RESET;
        bus_state = OFF_BUS;
    }
}

void can_set_bitrate(enum can_bitrate bitrate) {
    if (bus_state == ON_BUS) {
        // cannot set bitrate while on bus
        return;
    }

    switch (bitrate) {
        case CAN_BITRATE_500K:
            hcan.Init.Prescaler = 12;
            break;
        case CAN_BITRATE_250K:
            hcan.Init.Prescaler = 24;
            break;
        case CAN_BITRATE_125K:
            hcan.Init.Prescaler = 48;
            break;
    }
}

uint32_t can_tx(CanTxMsgTypeDef *tx_msg, uint32_t timeout) {
    uint32_t status;

    // transmit can frame
    hcan.pTxMsg = tx_msg;
    status = HAL_CAN_Transmit(&hcan, timeout);

    return status;
}

uint32_t can_rx(CanRxMsgTypeDef *rx_msg, uint32_t timeout) {
    uint32_t status;

    hcan.pRxMsg = rx_msg;

    status = HAL_CAN_Receive(&hcan, CAN_FIFO0, timeout);

    return status;
}

uint8_t is_can_msg_pending(uint8_t fifo) {
    if (bus_state == OFF_BUS) {
        return 0;
    }
    return (__HAL_CAN_MSG_PENDING(&hcan, fifo) > 0);
}
