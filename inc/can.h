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

typedef enum can_bus_state {
    OFF_BUS = 0,
    ON_BUS = 1,
} can_bus_state_t;


// CAN transmit buffering
#define TXQUEUE_LEN 28 // Number of buffers allocated
#define TXQUEUE_DATALEN 8 // CAN DLC length of data buffers

typedef struct cantxbuf_
{
	uint8_t data[TXQUEUE_LEN][TXQUEUE_DATALEN]; // Data buffer
	CAN_TxHeaderTypeDef header[TXQUEUE_LEN]; // Header buffer
	uint8_t head; // Head pointer
	uint8_t tail; // Tail pointer
	uint8_t full; // TODO: Set this when we are full, clear when the tail moves one.
} can_txbuf_t;


// Prototypes
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
