/*
 * bkit_app.c
 *
 * Description: Implementation of Application Layer
 */

#include "bkit_app.h"

// Select driver based on configuration in bkit_hw.h
#ifdef HW_DRIVER_UART
static bkit_interface_t *app_driver = &bkit_uart_driver;
#else
static bkit_interface_t *app_driver = &bkit_i2c_driver;
#endif

bool bkit_app_init(void) {
    // Initialize message processing layer, which initializes hardware driver
    if (bkit_process_init(app_driver) == BKIT_COM_OK) {
        return true;
    }
    return false;
}

bool bkit_send_message(const BKITSensorData* data) {
    if (data == NULL) return false;

    // Call Message Processing layer to encode, add Header, CRC and send
    // Cast const away because bkit_process_send doesn't take const currently (can be improved later)
    if (bkit_process_send((BKITSensorData*)data) == BKIT_COM_OK) {
        return true;
    }
    
    return false;
}

bool bkit_receive_message(BKITSensorData* data) {
    if (data == NULL) return false;

    // Call Message Processing layer to receive, check CRC and decode
    // bkit_process_receive returns BKIT_COM_OK ONLY when a complete packet is received
    if (bkit_process_receive(data) == BKIT_COM_OK) {
        return true; // Notify user that new data is available
    }

    return false; // No data available or error
}