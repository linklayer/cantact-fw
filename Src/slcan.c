#include "stm32f0xx_hal.h"
#include "can.h"
#include "slcan.h"


slcan_usb2can_fsm usb2canfsm;
CanTxMsgTypeDef frame;
uint8_t counter;
uint8_t bitrate = 9;
uint8_t mode = 0;
uint32_t current_filter_id = 0;
uint32_t current_filter_temp = 0;
uint32_t current_mask_id = 0;
uint32_t current_mask_temp = 0;

int8_t slcan_parse_frame(uint8_t *buf, CanRxMsgTypeDef *frame) {
    uint8_t i = 0;
    uint8_t id_len, j;
    uint32_t tmp;
  
  //warning: buf has to be smaller or equal to SLCAN_MTU. Potential bug
    memset(buf, '\0', SLCAN_MTU);

    // add character for frame type
    if (frame->RTR == CAN_RTR_DATA) {
        buf[i] = 't';
    } else if (frame->RTR == CAN_RTR_REMOTE) {
        buf[i] = 'r';
    }

    // assume standard identifier
    id_len = SLCAN_STD_ID_LEN;
    tmp = frame->StdId;
    // check if extended
    if (frame->IDE == CAN_ID_EXT) {
        // convert first char to upper case for extended frame
        buf[i] -= 32;
        id_len = SLCAN_EXT_ID_LEN;
        tmp = frame->ExtId;
    }
    i++;

    // add identifier to buffer
    for(j=id_len; j > 0; j--) {
        // add nybble to buffer
        buf[j] = (tmp & 0xF);
        tmp = tmp >> 4;
        i++;
    }

    // add DLC to buffer
    buf[i++] = frame->DLC;

    // add data bytes
    for (j = 0; j < frame->DLC; j++) {
        buf[i++] = (frame->Data[j] >> 4);
        buf[i++] = (frame->Data[j] & 0x0F);
    }

    // convert to ASCII (2nd character to end)
    for (j = 1; j < i; j++) {
        if (buf[j] < 0xA) {
            buf[j] += 0x30;
        } else {
            buf[j] += 0x37;
        }
    }

    // add carrage return (slcan EOL)
    buf[i++] = '\r';

    // return number of bytes in string
    return i;
}

slcan_usb2can_fsm parse_command_type(uint8_t command_type){
      switch (command_type){
      case 'O':
        return GET_OPEN_COMMAND;
      case 'C':
        return GET_CLOSE_COMMAND;
      case 'S':
        return GET_BITRATE_COMMAND;
      case 'm':
      case 'M':
        return GET_MODE_COMMAND;
      case 'F':
        return GET_FILTER_COMMAND;
      case 'K':
        return GET_MASK_COMMAND;
      case 't':
        frame.RTR = CAN_RTR_DATA;
        frame.IDE = CAN_ID_STD;
        return GET_FRAME_STD_ID;
      case 'r':
        frame.RTR = CAN_RTR_REMOTE;
        frame.IDE = CAN_ID_STD;
        return GET_FRAME_STD_ID;
      case 'T':
        frame.RTR = CAN_RTR_DATA;
        frame.IDE = CAN_ID_EXT;
        return GET_FRAME_EXT_ID;
      case 'R':
        frame.RTR = CAN_RTR_REMOTE;
        frame.IDE = CAN_ID_EXT;
        return GET_FRAME_EXT_ID;
      default:
        return GET_MSG_TYPE;
    }
}

uint8_t ASCII2byte(char ASCII_symbol){
      // convert from ASCII (2nd character to end)
   
    // lowercase letters
    if (ASCII_symbol >= 'a')
       return (ASCII_symbol - 'a' + 10);
    // uppercase letters
    else if(ASCII_symbol >= 'A')
       return (ASCII_symbol - 'A' + 10);
    // numbers
    else if ((ASCII_symbol <= '9') && (ASCII_symbol >= '0'))
      return ASCII_symbol - '0';
    else
      return 0xFF;
}

uint8_t switch_bitrate_command(uint8_t bitrate){
  switch(bitrate) {
    case 0:
       can_set_bitrate(CAN_BITRATE_10K);
       break;
    case 1:
       can_set_bitrate(CAN_BITRATE_20K);
       break;
    case 2:
       can_set_bitrate(CAN_BITRATE_50K);
       break;
    case 3:
       can_set_bitrate(CAN_BITRATE_100K);
       break;
    case 4:
       can_set_bitrate(CAN_BITRATE_125K);
       break;
    case 5:
       can_set_bitrate(CAN_BITRATE_250K);
       break;
    case 6:
       can_set_bitrate(CAN_BITRATE_500K);
       break;
    case 7:
       can_set_bitrate(CAN_BITRATE_750K);
       break;
    case 8:
       can_set_bitrate(CAN_BITRATE_1000K);
       break;
    default:
       // invalid setting
       return 0xFF;
  }
  return 0;
}

int8_t slcan_parse_str(uint8_t newbyte) {
    static volatile char byte; 
    byte = (char)newbyte;
    
    if ((usb2canfsm != GET_MSG_TYPE) && (newbyte != '\r'))
      newbyte = ASCII2byte(newbyte);
    
    switch (usb2canfsm){
      case GET_MSG_TYPE:
        //check which message type came
        usb2canfsm = parse_command_type(newbyte);
        
        break;
      case GET_OPEN_COMMAND:
        //if next byte is '\r', open channel and reset fsm
        if (newbyte == '\r'){
          can_enable();
        }
        usb2canfsm = GET_MSG_TYPE;
        return 0;
      case GET_CLOSE_COMMAND:
        //if next byte is '\r', open channel and reset fsm
        if (newbyte == '\r'){
          can_disable();
        }
        usb2canfsm = GET_MSG_TYPE;
        return 0;
      case GET_BITRATE_COMMAND:
        if (counter >= 1){
          if (newbyte == '\r')
            switch_bitrate_command(bitrate);
          counter = 0;
          usb2canfsm = GET_MSG_TYPE;
          return 0;
        } else{
          counter++;
          bitrate = newbyte;
        }
        break;
      case GET_MODE_COMMAND:
        if (counter >= 1){
          if (newbyte == '\r')
            can_set_silent(mode);
          counter = 0;
          usb2canfsm = GET_MSG_TYPE;
          return 0;
        } else{
          counter++;
          mode = newbyte;
        }
        break;
      case GET_FILTER_COMMAND:
        if (counter >= 5){
          if (newbyte == '\r'){
            current_filter_id = current_filter_temp;
            can_set_filter(current_filter_id, current_mask_id);
          }
          current_filter_temp = 0;
          counter = 0;
          usb2canfsm = GET_MSG_TYPE;
          return 0;
        } else{
          counter++;
          current_filter_temp = (current_filter_temp << 4) | newbyte;
        }
        break;
      case GET_MASK_COMMAND:
        if (counter >= 5){
          if (newbyte == '\r'){
            current_mask_id = current_mask_temp;
            can_set_filter(current_filter_id, current_mask_id);
          }
          current_mask_temp = 0;
          counter = 0;
          usb2canfsm = GET_MSG_TYPE;
          return 0;
        } else{
          counter++;
          current_mask_temp = (current_mask_temp << 4) | newbyte;
        }
        break;
      case GET_FRAME_STD_ID:
        if (counter >= 3){
          if ((newbyte <= 8) && (newbyte > 0)) {
            frame.DLC = newbyte;
            usb2canfsm = GET_FRAME_DATA;
          } else 
            usb2canfsm = GET_MSG_TYPE;
          counter = 0;
        } else{
          counter++;
          frame.StdId = (frame.StdId << 4) | newbyte;
        }
        break;
      case GET_FRAME_EXT_ID:
        if (counter >= 9){
          if (newbyte <= 8) {
            frame.DLC = newbyte;
            usb2canfsm = GET_FRAME_DATA;
          } else 
            usb2canfsm = GET_MSG_TYPE;
          counter = 0;
        } else{
          counter++;
          frame.ExtId = (frame.ExtId << 4) | newbyte;
        }
        break;
      case GET_FRAME_DATA:
        if (counter >= (frame.DLC*2)){
          if (newbyte == '\r'){
            can_tx(&frame, 0);
          }
          memset(&frame, 0x00, sizeof(frame));
          counter = 0;
          usb2canfsm = GET_MSG_TYPE;
          return 0;
        } else{
          frame.Data[counter/2] = (frame.Data[counter/2] << 4) | newbyte;
          counter++;
        }
        break;
      default:
        usb2canfsm = GET_MSG_TYPE;
        break;
    }
    
    if (byte == '\r'){
      usb2canfsm = GET_MSG_TYPE;
      counter = 0;
      return 0;
    }

    return 1;
}
