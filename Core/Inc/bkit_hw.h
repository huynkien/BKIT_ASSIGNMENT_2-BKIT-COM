/*
 * bkit_hw.h
 *
 *  Created on: Jan 25, 2026
 *      Author: Dell
 */

#ifndef INC_BKIT_HW_H_
#define INC_BKIT_HW_H_

#include "main.h"

/* BKIT-COM status definitions:
 * BKIT_COM_OK:
 *  - Init:    UART peripheral initialized and Interrupt enabled successfully.
 *  - Send:    Transmission completed successfully 
 *  - Receive: Buffer processing finished 
 *
 * BKIT_COM_ERROR:
 *  - Init:    Failed to enable UART/Interrupt (HAL_UART_Receive_IT returned error).
 *
 * BKIT_COM_BUSY:
 *  - Send:    UART hardware is currently transmitting another frame.
 */

typedef enum {
	BKIT_COM_OK,
	BKIT_COM_ERROR,	
	BKIT_COM_BUSY,
	BKIT_COM_RX_OVERFLOW
} bkit_status_t;




/* @brief Struct for protocol hardware interface layer
 * - hw_init: Initialize the peripheral (UART, ...).
 * - hw_send: Transmit a sequence of bytes.
 * - hw_receive: Read available data from the internal driver buffer (Ring Buffer) into the user application buffer.
 */

typedef struct {
	bkit_status_t (*hw_init)(void);
	bkit_status_t (*hw_send)(uint8_t* msg, uint16_t length);
	bkit_status_t (*hw_receive)(uint8_t* buffer, uint16_t length, uint16_t *received_msg_length);
} bkit_interface_t;

extern bkit_interface_t bkit_uart_driver;

#endif /* INC_BKIT_HW_H_ */
