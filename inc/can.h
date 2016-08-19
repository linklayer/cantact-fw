#ifndef _CAN_H
#define _CAN_H

enum can_bitrate {
    CAN_BITRATE_10K,
    CAN_BITRATE_20K,
    CAN_BITRATE_50K,
    CAN_BITRATE_100K,
    CAN_BITRATE_125K,
    CAN_BITRATE_250K,
    CAN_BITRATE_500K,
    CAN_BITRATE_750K,
    CAN_BITRATE_1000K,
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
uint32_t can_tx(CanTxMsgTypeDef *tx_msg);

void can_process(void);
void can_preptx(CanTxMsgTypeDef *tx_msg);

uint8_t is_can_msg_pending(uint8_t fifo);
CAN_HandleTypeDef* can_gethandle(void);

#endif // _CAN_H
