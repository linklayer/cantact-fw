//
// slcan: Parse incoming and generate outgoing slcan messages

#include "stm32f0xx_hal.h"
#include "can.h"
#include "slcan.h"

int8_t slcan_parse_frame(uint8_t *buf, CanRxMsgTypeDef *frame)
{
    uint8_t i = 0;
    uint8_t id_len, j;
    uint32_t tmp;

    for (j=0; j < SLCAN_MTU; j++)
    {
        buf[j] = '\0';
    }

    // add character for frame type
    if (frame->RTR == CAN_RTR_DATA)
    {
        buf[i] = 't';
    } else if (frame->RTR == CAN_RTR_REMOTE) {
        buf[i] = 'r';
    }

    // assume standard identifier
    id_len = SLCAN_STD_ID_LEN;
    tmp = frame->StdId;

    // check if extended
    if (frame->IDE == CAN_ID_EXT)
    {
        // convert first char to upper case for extended frame
        buf[i] -= 32;
        id_len = SLCAN_EXT_ID_LEN;
        tmp = frame->ExtId;
    }
    i++;

    // add identifier to buffer
    for(j=id_len; j > 0; j--)
    {
        // add nybble to buffer
        buf[j] = (tmp & 0xF);
        tmp = tmp >> 4;
        i++;
    }

    // add DLC to buffer
    buf[i++] = frame->DLC;

    // add data bytes
    for (j = 0; j < frame->DLC; j++)
    {
        buf[i++] = (frame->Data[j] >> 4);
        buf[i++] = (frame->Data[j] & 0x0F);
    }

    // convert to ASCII (2nd character to end)
    for (j = 1; j < i; j++)
    {
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


int8_t slcan_parse_str(uint8_t *buf, uint8_t len)
{
    CanTxMsgTypeDef frame;
    uint8_t i;

    // convert from ASCII (2nd character to end)
    for (i = 1; i < len; i++)
    {
        // lowercase letters
        if(buf[i] >= 'a')
            buf[i] = buf[i] - 'a' + 10;
        // uppercase letters
        else if(buf[i] >= 'A')
            buf[i] = buf[i] - 'A' + 10;
        // numbers
        else
            buf[i] = buf[i] - '0';
    }

    if (buf[0] == 'O')
    {
        // open channel command
        can_enable();
        return 0;

    } else if (buf[0] == 'C') {
        // close channel command
        can_disable();
        return 0;

    } else if (buf[0] == 'S') {
        // set bitrate command
        switch(buf[1])
        {
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
                return -1;
        }
        return 0;

    } else if (buf[0] == 'm' || buf[0] == 'M') {
        // set mode command
        if (buf[1] == 1) 
        {
            // mode 1: silent
            can_set_silent(1);
        } else {
            // default to normal mode
            can_set_silent(0);
        }
        return 0;

    } else if (buf[0] == 't' || buf[0] == 'T') {
        // transmit data frame command
        frame.RTR = CAN_RTR_DATA;

    } else if (buf[0] == 'r' || buf[0] == 'R') {
        // transmit remote frame command
        frame.RTR = CAN_RTR_REMOTE;

    } else {
        // error, unknown command
        return -1;
    }

    if (buf[0] == 't' || buf[0] == 'r') {
        frame.IDE = CAN_ID_STD;
    } else if (buf[0] == 'T' || buf[0] == 'R') {
        frame.IDE = CAN_ID_EXT;
    } else {
        // error
        return -1;
    }

    frame.StdId = 0;
    frame.ExtId = 0;
    if (frame.IDE == CAN_ID_EXT) {
        uint8_t id_len = SLCAN_EXT_ID_LEN;
        i = 1;
        while (i <= id_len) {
            frame.ExtId *= 16;
            frame.ExtId += buf[i++];
        }
    }
    else {
        uint8_t id_len = SLCAN_STD_ID_LEN;
        i = 1;
        while (i <= id_len) {
            frame.StdId *= 16;
            frame.StdId += buf[i++];
        }
    }


    frame.DLC = buf[i++];
    if (frame.DLC < 0 || frame.DLC > 8) {
        return -1;
    }

    uint8_t j;
    for (j = 0; j < frame.DLC; j++) {
        frame.Data[j] = (buf[i] << 4) + buf[i+1];
        i += 2;
    }

    // send the message
    can_preptx(&frame);

    return 0;
}
