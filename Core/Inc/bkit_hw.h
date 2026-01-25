/*
 * bkit_hw.h
 *
 *  Created on: Jan 25, 2026
 *      Author: Dell
 */

#ifndef INC_BKIT_HW_H_
#define INC_BKIT_HW_H_

#include "main.h"

typedef enum {
	BKIT_COM_OK,
	BKIT_COM_ERROR,	
	BKIT_COM_BUSY
} bkit_status_t;

// Struct for protocol hardware interface layer
typedef struct {
	bkit_status_t (*hw_init)(void);
	bkit_status_t (*hw_send_byte)(uint8_t* data, uint16_t length);
	bkit_status_t (*hw_receive_byte)(uint8_t* buffer, uint16_t length, uint16_t *received_msg_length);
} bkit_interface_t;

extern bkit_interface_t bkit_uart_driver;

#endif /* INC_BKIT_HW_H_ */
