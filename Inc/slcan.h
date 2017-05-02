#ifndef _SLCAN_H
#define _SLCAN_H

#include <string.h>

int8_t slcan_parse_frame(uint8_t *buf, CanRxMsgTypeDef *frame);
int8_t slcan_parse_str(uint8_t newbyte);

typedef enum{
  GET_MSG_TYPE = 0,
  GET_FRAME_STD_ID,
  GET_FRAME_EXT_ID,
  GET_FRAME_DATA,
  GET_OPEN_COMMAND,
  GET_CLOSE_COMMAND,
  GET_BITRATE_COMMAND,
  GET_MODE_COMMAND,
  GET_FILTER_COMMAND,
  GET_MASK_COMMAND
} slcan_usb2can_fsm;

/* maximum rx buffer len: extended CAN frame with timestamp */
#define SLCAN_MTU 128// (sizeof("T1111222281122334455667788EA5F\r")+1)

#define SLCAN_STD_ID_LEN 3
#define SLCAN_EXT_ID_LEN 8

#endif // _SLCAN_H
