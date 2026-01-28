/*
 * message_processing.h
 *
 *  Created on: Jan 27, 2026
 *      Author: Dell
 */

#ifndef INC_MESSAGE_PROCESSING_H_
#define INC_MESSAGE_PROCESSING_H_

#include "bkit_hw.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "sensor.pb.h"
#include "crc16.h"
#include <string.h>

#define START_BYTE			0x99
#define HEADER_LENGTH 		8
#define PACKET_MAX_LENGTH  	BKITSensorData_size + HEADER_LENGTH

typedef enum {
    WAIT_START_BYTE,	// Wait for start byte of packet - 0x99
    WAIT_HEADER,		// Read header
    WAIT_PAYLOAD		// Read payload
} processing_receive_t;

uint16_t 	  get_device_id(void);
bkit_status_t bkit_process_init(bkit_interface_t  *driver);
bkit_status_t bkit_process_send(BKITSensorData    *data);
bkit_status_t bkit_process_receive(BKITSensorData *data);

#endif /* INC_MESSAGE_PROCESSING_H_ */
