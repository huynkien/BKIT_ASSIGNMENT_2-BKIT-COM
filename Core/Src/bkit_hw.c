/*
 * bkit_hw.c
 *
 *  Created on: Jan 25, 2026
 *      Author: Dell
 */

#include "bkit_hw.h"
#include "usart.h"

#define RX_BUFFER_SIZE 256

static uint8_t  rx_buffer[RX_BUFFER_SIZE] = {0};
static uint16_t volatile head_ptr = 0;				// pointer to write message to buffer
static uint16_t volatile tail_ptr = 0;				// pointer to read message from buffer
static uint8_t  rx_temp_byte = 0;					// read temporary received byte
static uint8_t  volatile rx_overflow_flag = 0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART6) { // read byte when have signal
        uint16_t next_head = head_ptr + 1;
        if (next_head >= RX_BUFFER_SIZE) {
            next_head = 0;
        }
		// Check if buffer is full or not
        if (next_head != tail_ptr) { //buffer is not full
            rx_buffer[head_ptr] = rx_temp_byte; 
            head_ptr = next_head;
			rx_overflow_flag = 0;
        } 
        else { //buffer is full
			rx_overflow_flag = 1; //There are not specific case to process this warning :>
        }
		HAL_UART_Receive_IT(&huart6, &rx_temp_byte, 1);
	}
}



/**
  * @brief  Initialize the peripheral (UART, ...).
  * @param  None
  * @retval BKIT-COM status.
  */

bkit_status_t hw_init_uart() {
	if (HAL_UART_Receive_IT(&huart6, &rx_temp_byte, 1) != HAL_OK) {
		return BKIT_COM_ERROR;
	}
	return BKIT_COM_OK;
}

/**
  * @brief  Transmit a sequence of bytes.
  * @param  msg Pointer to the data buffer to send.
  * @param  length Number of bytes to send.
  * @retval BKIT-COM status.
  */

bkit_status_t hw_send_uart(uint8_t *msg, uint16_t length) {
	if (HAL_UART_Transmit(&huart6, msg, length, 256) != HAL_OK) {
		return BKIT_COM_BUSY;
	}
	return BKIT_COM_OK;
}

/**
  * @brief  Read available data from the internal driver buffer (Ring Buffer)
  * 		into the user application buffer.
  * @param  buffer Pointer to the user's destination buffer.
  * @param  max_length Maximum capacity of the user's buffer.
  * @param  received_msg_length Output pointer, the driver writes
  * 							actual number of bytes read into this variable.
  * @retval BKIT-COM status.
  */

bkit_status_t hw_receive_uart(uint8_t *buffer, uint16_t max_length, uint16_t *received_msg_length) {
	uint16_t count = 0;
	*received_msg_length = 0;
	while (tail_ptr != head_ptr && count < max_length) {
		buffer[count++] = rx_buffer[tail_ptr++];
		if (tail_ptr >= RX_BUFFER_SIZE) {
			tail_ptr = 0;
		}
	}
	*received_msg_length = count;
	return BKIT_COM_OK;
}

bkit_interface_t bkit_uart_driver = {
	.hw_init = hw_init_uart,
	.hw_send = hw_send_uart,
	.hw_receive = hw_receive_uart
};
