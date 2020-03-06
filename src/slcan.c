//
// slcan: Parse incoming and generate outgoing slcan messages
//

#include "stm32f0xx_hal.h"
#include <string.h>
#include "can.h"
#include "slcan.h"
#include "usbd_cdc_if.h"


// Parse an incoming CAN frame into an outgoing slcan message
int8_t slcan_parse_frame(uint8_t *buf, CAN_RxHeaderTypeDef *frame_header, uint8_t* frame_data)
{
	// TODO: Rename these variables
    uint8_t i = 0;
    uint32_t tmp;

    for (uint8_t j=0; j < SLCAN_MTU; j++)
    {
        buf[j] = '\0';
    }

    // Add character for frame type
    if (frame_header->RTR == CAN_RTR_DATA)
    {
        buf[i] = 't';
    } else if (frame_header->RTR == CAN_RTR_REMOTE) {
        buf[i] = 'r';
    }

    // Assume standard identifier
    uint8_t id_len = SLCAN_STD_ID_LEN;
    tmp = frame_header->StdId;

    // Check if extended
    if (frame_header->IDE == CAN_ID_EXT)
    {
        // Convert first char to upper case for extended frame
        buf[i] -= 32;
        id_len = SLCAN_EXT_ID_LEN;
        tmp = frame_header->ExtId;
    }
    i++;

    // Add identifier to buffer
    for(uint8_t j = id_len; j > 0; j--)
    {
        // Add nybble to buffer
        buf[j] = (tmp & 0xF);
        tmp = tmp >> 4;
        i++;
    }

    // Add DLC to buffer
    buf[i++] = frame_header->DLC;

    // Add data bytes
    for (uint8_t j = 0; j < frame_header->DLC; j++)
    {
        buf[i++] = (frame_data[j] >> 4);
        buf[i++] = (frame_data[j] & 0x0F);
    }

    // Convert to ASCII (2nd character to end)
    for (uint8_t j = 1; j < i; j++)
    {
        if (buf[j] < 0xA) {
            buf[j] += 0x30;
        } else {
            buf[j] += 0x37;
        }
    }

    // Add carrage return (slcan EOL)
    buf[i++] = '\r';

    // Return number of bytes in string
    return i;
}


// Parse an incoming slcan command from the USB CDC port
int8_t slcan_parse_str(uint8_t *buf, uint8_t len)
{
	CAN_TxHeaderTypeDef frame_header;
    uint8_t frame_data[8] = {0};

    // Rename
    uint8_t i;

    // Convert from ASCII (2nd character to end)
    for (uint8_t i = 1; i < len; i++)
    {
        // Lowercase letters
        if(buf[i] >= 'a')
            buf[i] = buf[i] - 'a' + 10;
        // Uppercase letters
        else if(buf[i] >= 'A')
            buf[i] = buf[i] - 'A' + 10;
        // Numbers
        else
            buf[i] = buf[i] - '0';
    }

    if (buf[0] == 'O')
    {
        // Open channel command
        can_enable();
        return 0;

    } else if (buf[0] == 'C') {
        // Close channel command
        can_disable();
        return 0;

    } else if (buf[0] == 'S') {
        // Set bitrate command

    	// Check for valid bitrate
    	if(buf[1] >= CAN_BITRATE_INVALID)
    	{
    		return -1;
    	}

		can_set_bitrate(buf[1]);
        return 0;

    } else if (buf[0] == 'm' || buf[0] == 'M') {
        // Set mode command
        if (buf[1] == 1) 
        {
            // Mode 1: silent
            can_set_silent(1);
        } else {
            // Default to normal mode
            can_set_silent(0);
        }
        return 0;

    } else if (buf[0] == 'a' || buf[0] == 'A') {
        // Set autoretry command
        if (buf[1] == 1) 
        {
            // Mode 1: autoretry enabled (default)
            can_set_autoretransmit(1);
        } else {
            // Mode 0: autoretry disabled
            can_set_autoretransmit(0);
        }
        return 0;

    } else if (buf[0] == 'v' || buf[0] == 'V') {
        // Report firmware version and remote
        char* fw_id = GIT_VERSION " " GIT_REMOTE "\r";
        CDC_Transmit_FS((uint8_t*)fw_id, strlen(fw_id));
        return 0;

    } else if (buf[0] == 't' || buf[0] == 'T') {
        // Transmit data frame command
        frame_header.RTR = CAN_RTR_DATA;

    } else if (buf[0] == 'r' || buf[0] == 'R') {
        // Transmit remote frame command
    	frame_header.RTR = CAN_RTR_REMOTE;

    } else {
        // Error, unknown command
        return -1;
    }

    // Check for extended or standard frame (set by case)
    if (buf[0] == 't' || buf[0] == 'r') {
    	frame_header.IDE = CAN_ID_STD;
    } else if (buf[0] == 'T' || buf[0] == 'R') {
    	frame_header.IDE = CAN_ID_EXT;
    } else {
        // error
        return -1;
    }

    frame_header.StdId = 0;
    frame_header.ExtId = 0;
    if (frame_header.IDE == CAN_ID_EXT) {
        uint8_t id_len = SLCAN_EXT_ID_LEN;
        i = 1;
        while (i <= id_len) {
        	frame_header.ExtId *= 16;
        	frame_header.ExtId += buf[i++];
        }
    }
    else {
        uint8_t id_len = SLCAN_STD_ID_LEN;
        i = 1;
        while (i <= id_len) {
        	frame_header.StdId *= 16;
        	frame_header.StdId += buf[i++];
        }
    }


    // Attempt to parse DLC and check sanity
    frame_header.DLC = buf[i++];
    if (frame_header.DLC < 0 || frame_header.DLC > 8) {
        return -1;
    }

    uint8_t j;
    for (j = 0; j < frame_header.DLC; j++) {
        frame_data[j] = (buf[i] << 4) + buf[i+1];
        i += 2;
    }

    // Transmit the message
    can_tx(&frame_header, frame_data);

    return 0;
}

