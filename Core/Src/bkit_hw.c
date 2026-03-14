/*
 * bkit_hw.c
 *
 *  Created on: Jan 25, 2026
 *      Author: Dell
 */

#include "bkit_hw.h"

static uint8_t  rx_buffer[RX_BUFFER_SIZE] = {0};
static uint8_t  volatile rx_overflow_flag = 0;
static uint16_t volatile head_ptr = 0; 	// pointer to write message to UART receive buffer
static uint16_t volatile tail_ptr = 0; 	// pointer to read message from UART receive buffer
static uint8_t  rx_temp_byte = 0;      	// temporary received byte

#ifdef HW_DRIVER_UART

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART6) {
		uint16_t next_head = head_ptr + 1;
		if (next_head >= RX_BUFFER_SIZE) {
			next_head = 0;
		}
		if (next_head != tail_ptr) {
			rx_buffer[head_ptr] = rx_temp_byte;
			head_ptr = next_head;
			rx_overflow_flag = 0;
		} else {
			rx_overflow_flag = 1;
		}
		HAL_UART_Receive_IT(&huart6, &rx_temp_byte, 1);
	}
}
#else /* HW_DRIVER_I2C_IT */

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c->Instance == I2C2) {
		uint16_t next_head = head_ptr + 1;
		if (next_head >= RX_BUFFER_SIZE) {
			next_head = 0;
		}
		if (next_head != tail_ptr) {
			rx_buffer[head_ptr] = rx_temp_byte;
			head_ptr = next_head;
			rx_overflow_flag = 0;
		} else {
			rx_overflow_flag = 1;
		}
		HAL_I2C_Master_Receive_IT(&hi2c2, BKIT_I2C_ADDRESS << 1, &rx_temp_byte, 1);
	}
}
#endif

/**
  * @brief  Initialize the peripheral (UART).
  * @param  None
  * @retval BKIT-COM status.
  */
#ifdef HW_DRIVER_UART
bkit_status_t hw_init_uart() {
	if (HAL_UART_Receive_IT(&huart6, &rx_temp_byte, 1) != HAL_OK) {
		return BKIT_COM_ERROR;
	}
	return BKIT_COM_OK;
}
#else /* HW_DRIVER_I2C */
/**
  * @brief  Initialize the peripheral (I2C).
  * @param  None
  * @retval BKIT-COM status.
  */
bkit_status_t hw_init_i2c() {
	// Start receiving data interrupt
	if (HAL_I2C_Master_Receive_IT(&hi2c2, BKIT_I2C_ADDRESS << 1, &rx_temp_byte, 1) != HAL_OK) {
		return BKIT_COM_ERROR;
	}
	return BKIT_COM_OK;
}
#endif

/**
  * @brief  Transmit a sequence of bytes. (UART)
  * @param  msg Pointer to the data buffer to send.
  * @param  length Number of bytes to send.
  * @retval BKIT-COM status.
  */
#ifdef HW_DRIVER_UART
bkit_status_t hw_send_uart(uint8_t *msg, uint16_t length) {
	if (msg == NULL || length == 0)
		return BKIT_COM_ERROR;
	if (HAL_UART_Transmit(&huart6, msg, length, 1000) != HAL_OK)
		return BKIT_COM_BUSY;
	return BKIT_COM_OK;
}
#else /* HW_DRIVER_I2C */
/**
  * @brief  Transmit a sequence of bytes (I2C).
  * @param  msg Pointer to the data buffer to send.
  * @param  length Number of bytes to send.
  * @retval BKIT-COM status.
  */
bkit_status_t hw_send_i2c(uint8_t *msg, uint16_t length) {
	if (msg == NULL || length == 0)
		return BKIT_COM_ERROR;
	
	if (HAL_I2C_Master_Transmit(&hi2c2, BKIT_I2C_ADDRESS << 1, msg, length, 1000) != HAL_OK)
		return BKIT_COM_ERROR;
	return BKIT_COM_OK;
}
#endif

/**
  * @brief  Read available data from the internal driver256 buffer (Ring Buffer)
  * 		into the user application buffer. (UART)
  * @param  buffer Pointer to the user's destination buffer.
  * @param  max_length Maximum capacity of the user's buffer.
  * @param  received_msg_length Output pointer, the driver writes
  * 							actual number of bytes read into this variable.
  * @retval BKIT-COM status.
  */
#ifdef HW_DRIVER_UART
bkit_status_t hw_receive_uart(uint8_t *buffer, uint16_t max_length, uint16_t *received_msg_length) {
	if (buffer == NULL || received_msg_length == NULL)
		return BKIT_COM_ERROR;

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
#else /* HW_DRIVER_I2C */
/**
  * @brief  Receive bytes (I2C).
  * @param  buffer Pointer to the user's destination buffer.
  * @param  max_length Maximum capacity of the user's buffer.
  * @param  received_msg_length Output pointer, the driver writes
  * 							actual number of bytes read into this variable.
  * @retval BKIT-COM status.
  */
bkit_status_t hw_receive_i2c(uint8_t *buffer, uint16_t max_length, uint16_t *received_msg_length) {
	if (buffer == NULL || received_msg_length == NULL)
		return BKIT_COM_ERROR;

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
#endif

/* ------------------ Driver instances ------------------ */
#ifdef HW_DRIVER_UART
bkit_interface_t bkit_uart_driver = {
	.hw_init = hw_init_uart,
	.hw_send = hw_send_uart,
	.hw_receive = hw_receive_uart
};
#else /* HW_DRIVER_I2C */
bkit_interface_t bkit_i2c_driver = {
	.hw_init = hw_init_i2c,
	.hw_send = hw_send_i2c,
	.hw_receive = hw_receive_i2c
};
#endif
