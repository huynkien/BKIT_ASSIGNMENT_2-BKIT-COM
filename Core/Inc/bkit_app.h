/*
 * bkit_app.h
 * Author: levandinhhuy
 * Description: Application Layer for BKIT-COM Protocol
 * Implements high-level APIs as required in Assignment Section 2.4
 */

#ifndef INC_BKIT_APP_H_
#define INC_BKIT_APP_H_

#include <stdbool.h>
#include <stdint.h>
#include "message_processing.h"

/**
 * @brief  Initialize the application and underlying protocol layers.
 * @return true if initialization is successful, false otherwise.
 */
bool bkit_app_init(void);

/**
 * @brief  Send a message containing sensor data.
 * @param  data: Pointer to BKITSensorData struct to send (Read-only).
 * @return true if sent successfully, false otherwise.
 */
bool bkit_send_message(const BKITSensorData* data);

/**
 * @brief  Receive message and decode sensor data.
 * @param  data: Pointer to BKITSensorData struct to store received data.
 * @return true if a valid packet is received, false if no packet or error.
 */
bool bkit_receive_message(BKITSensorData* data);

#endif /* INC_BKIT_APP_H_ */
