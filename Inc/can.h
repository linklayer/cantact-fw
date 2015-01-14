#ifndef _CAN_H
#define _CAN_H

enum can_bitrate {
    CAN_BITRATE_500K,
    CAN_BITRATE_250K,
    CAN_BITRATE_125K,
};

enum can_bus_state {
    OFF_BUS,
    ON_BUS
};

void can_init(void);
void can_enable(void);
void can_disable(void);
void can_set_bitrate(enum can_bitrate bitrate);
uint32_t can_tx(CanTxMsgTypeDef *tx_msg, uint32_t timeout);

#endif // _CAN_H
