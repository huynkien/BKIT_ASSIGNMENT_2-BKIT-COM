/*
 * message_processing.c
 *
 *  Created on: Jan 27, 2026
 *      Author: Dell
 */

#include "message_processing.h"

/* ------------------      GENERAL VARIABLE        ------------------ */
static bkit_interface_t *pr_driver = NULL;
uint8_t flag = 0x00;

/* ------------------ VARIABLE FOR RECEIVE PROCESS ------------------ */
static uint8_t  receive_buf[RX_BUFFER_SIZE];// Buffer to save data temporarily
static uint16_t receive_index = 0;       	// Saving position

uint16_t get_device_id(void) {
    uint32_t uid[3];

    uid[0] = HAL_GetUIDw0();
    uid[1] = HAL_GetUIDw1();
    uid[2] = HAL_GetUIDw2();

    return crc16((uint8_t *)uid, sizeof(uid));
}



bkit_status_t bkit_process_init(bkit_interface_t *driver) {
	if (driver == NULL || driver->hw_init == NULL)
		return BKIT_COM_ERROR;

	pr_driver = driver;
	return pr_driver->hw_init();
}



bkit_status_t bkit_process_send(BKITSensorData *data) {
	if(pr_driver == NULL || pr_driver->hw_send == NULL)
		return BKIT_COM_ERROR;

	uint8_t  payload_buf[BKITSensorData_size]; 	// 44
	uint8_t  packet_buf[PACKET_MAX_LENGTH]; 	// 44 + 1 + 2 + 2 + 2 + 1
	uint16_t hw_uid = get_device_id();

	pb_ostream_t stream = pb_ostream_from_buffer(payload_buf, sizeof(payload_buf));
	if (!pb_encode(&stream, BKITSensorData_fields, data))
		return BKIT_COM_ERROR;

	uint16_t payload_len = stream.bytes_written;
	// Calculate CRC
	uint16_t crc_check   = crc16(payload_buf, payload_len);
	//

	packet_buf[0] = START_BYTE;					// START OF PACKET
	packet_buf[1] = hw_uid      & 0xFF;			// 16bit UID
	packet_buf[2] = (hw_uid     >> 8) & 0xFF;
	packet_buf[3] = payload_len & 0xFF;			// 16bit payload length
	packet_buf[4] = (payload_len >> 8) & 0xFF;
	packet_buf[5] = crc_check   & 0xFF;			// 16bit CRC
	packet_buf[6] = (crc_check   >> 8) & 0xFF;
	packet_buf[7] = flag;						// 8 bit flag

	memcpy(&packet_buf[8], payload_buf, payload_len);

	return pr_driver->hw_send(packet_buf, payload_len + 8);
}



bkit_status_t bkit_process_receive(BKITSensorData *data) {
	if(pr_driver == NULL || pr_driver->hw_receive == NULL)
		return BKIT_COM_ERROR;

	static processing_receive_t receive_state = WAIT_START_BYTE;
	static uint16_t payload_receive_length = 0;
	static uint16_t receive_crc = 0;

	uint8_t  byte = 0;
	uint16_t num_byte_read = 0;

	// Process payload byte-by-byte 
	while (pr_driver->hw_receive(&byte, 1, &num_byte_read) == BKIT_COM_OK && num_byte_read > 0) {
		switch (receive_state) {
			case WAIT_START_BYTE:
				if (byte == START_BYTE) {
					receive_index = 0;
					receive_buf[receive_index++] = byte;
					receive_state = WAIT_HEADER;
				}
				break;

			case WAIT_HEADER:
				receive_buf[receive_index++] = byte;
				if (receive_index >= HEADER_LENGTH) { // full header collected
					payload_receive_length = receive_buf[3] | (receive_buf[4] << 8);
					receive_crc            = receive_buf[5] | (receive_buf[6] << 8);

					if (payload_receive_length == 0 ||
						payload_receive_length > BKITSensorData_size ||
						(payload_receive_length + HEADER_LENGTH) > PACKET_MAX_LENGTH) {
						receive_state = WAIT_START_BYTE;
						receive_index = 0;
					} else {
						receive_index = 0; // index 0 to easy to control
						receive_state = WAIT_PAYLOAD;
					}
				}
				break;

			case WAIT_PAYLOAD:
				receive_buf[receive_index++] = byte;
				if (receive_index >= payload_receive_length) {
					uint16_t crc_check = crc16(receive_buf, payload_receive_length);
					if (crc_check == receive_crc) {
						pb_istream_t stream = pb_istream_from_buffer(receive_buf, payload_receive_length);
						if (pb_decode(&stream, BKITSensorData_fields, data)) {
							receive_state = WAIT_START_BYTE;
							receive_index = 0;
							return BKIT_COM_OK;
						}
					}
					// CRC/decode failed 
					receive_state = WAIT_START_BYTE;
					receive_index = 0;
				}
				break;
		}
	}

	// No complete packet this round
	return BKIT_COM_OK;
}


