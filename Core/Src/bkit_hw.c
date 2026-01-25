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

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART6) {
		rx_buffer[head_ptr] = rx_temp_byte;
		head_ptr++;
		if (head_ptr >= RX_BUFFER_SIZE) {
			head_ptr = 0;
		}
		HAL_UART_Receive_IT(&huart6, &rx_temp_byte, 1);
	}
}

bkit_status_t hw_init_uart() {
	if (HAL_UART_Receive_IT(&huart6, &rx_temp_byte, 1) != HAL_OK) {
		return BKIT_COM_ERROR;
	}
	return BKIT_COM_OK;
}

bkit_status_t hw_send_uart(uint8_t *msg, uint16_t length) {
	if (HAL_UART_Transmit(&huart6, msg, length, 256) != HAL_OK) {
		return BKIT_COM_BUSY;
	}
	return BKIT_COM_OK;
}

bkit_status_t hw_receive_uart(uint8_t *buffer, uint16_t max_length, uint16_t received_msg_length) {
	uint16_t count = 0;
	while (tail_ptr != head_ptr && count < max_length) {
		buffer[count] = rx_buffer[tail_ptr];
		tail_ptr++;
		count++;
		if (tail_ptr >= RX_BUFFER_SIZE) {
			tail_ptr = 0;
		}
		*received_msg_length = count;
	}
	return BKIT_COM_OK;
}

bkit_interface_t bkit_uart_driver = {
	.hw_init = hw_init_uart,
	.hw_send_byte = hw_send_uart,
	.hw_receive_byte = hw_receive_uart
};
