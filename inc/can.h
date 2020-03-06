#ifndef _CAN_H
#define _CAN_H

enum can_bitrate {
    CAN_BITRATE_10K = 0,
    CAN_BITRATE_20K,
    CAN_BITRATE_50K,
    CAN_BITRATE_100K,
    CAN_BITRATE_125K,
    CAN_BITRATE_250K,
    CAN_BITRATE_500K,
    CAN_BITRATE_750K,
    CAN_BITRATE_1000K,

	CAN_BITRATE_INVALID,
};

enum can_bus_state {
    OFF_BUS,
    ON_BUS
};

void can_init(void);
void can_enable(void);
void can_disable(void);
void can_set_bitrate(enum can_bitrate bitrate);
void can_set_silent(uint8_t silent);
void can_set_autoretransmit(uint8_t autoretransmit);
uint32_t can_tx(CAN_TxHeaderTypeDef *tx_msg_header, uint8_t *tx_msg_data);
uint32_t can_rx(CAN_RxHeaderTypeDef *rx_msg_header, uint8_t *rx_msg_data);


void can_process(void);

uint8_t is_can_msg_pending(uint8_t fifo);
CAN_HandleTypeDef* can_gethandle(void);

#endif // _CAN_H
