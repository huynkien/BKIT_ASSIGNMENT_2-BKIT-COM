/*
 * bkit_app.h
 *
 * Description: Application Layer for BKIT-COM Protocol
 * Implements high-level APIs as required in Assignment Section 2.4
 */

#ifndef INC_BKIT_APP_H_
#define INC_BKIT_APP_H_

#include <stdbool.h>
#include <stdint.h>

/* * 1. Data Structures 
 * Định nghĩa cấu trúc dữ liệu người dùng (User-facing struct).
 * Cấu trúc này độc lập với Protobuf để người dùng dễ thao tác.
 */
typedef struct {
    uint32_t sensor_id;
    float    value;
    uint64_t timestamp;
} sensor_data_t;

/*
 * 2. High-Level APIs [cite: 123-126]
 * Các hàm này che giấu chi tiết kỹ thuật bên dưới (CRC, Protobuf, Hardware).
 */

/**
 * @brief  Khởi tạo ứng dụng và các lớp giao thức bên dưới.
 * @return true nếu khởi tạo thành công, false nếu lỗi.
 */
bool bkit_app_init(void);

/**
 * @brief  Gửi tin nhắn chứa dữ liệu cảm biến [cite: 125]
 * @param  data: Con trỏ tới struct chứa dữ liệu cần gửi (Read-only).
 * @return true nếu gửi thành công, false nếu lỗi.
 */
bool bkit_send_message(const sensor_data_t* data);

/**
 * @brief  Nhận tin nhắn và giải mã dữ liệu cảm biến [cite: 126]
 * @param  data: Con trỏ tới struct để lưu dữ liệu nhận được.
 * @return true nếu NHẬN ĐƯỢC gói tin hợp lệ, false nếu chưa có gói tin hoặc lỗi.
 */
bool bkit_receive_message(sensor_data_t* data);

#endif /* INC_BKIT_APP_H_ */