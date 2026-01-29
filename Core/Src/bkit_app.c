/*
 * bkit_app.c
 *
 * Description: Implementation of Application Layer
 */

#include "bkit_app.h"
#include "message_processing.h" // Gọi lớp dưới [cite: 121]

// Chọn driver mặc định (được khai báo extern trong bkit_hw.h)
static bkit_interface_t *app_driver = &bkit_uart_driver;

bool bkit_app_init(void) {
    // Khởi tạo lớp xử lý tin nhắn, lớp này sẽ khởi tạo driver phần cứng
    if (bkit_process_init(app_driver) == BKIT_COM_OK) {
        return true;
    }
    return false;
}

bool bkit_send_message(const sensor_data_t* data) {
    if (data == NULL) return false;

    /* Bước 1: Chuyển đổi từ App Struct sang Protobuf Struct [cite: 133]
     * sensor_data_t (App Layer) -> BKITSensorData (Message Layer)
     */
    BKITSensorData proto_msg = BKITSensorData_init_zero;
    
    proto_msg.sensor_id    = data->sensor_id;
    proto_msg.sensor_value = data->value;
    proto_msg.timestamp    = data->timestamp;

    /* Bước 2: Gọi lớp Message Processing để encode, thêm Header, CRC và gửi [cite: 127-130] */
    if (bkit_process_send(&proto_msg) == BKIT_COM_OK) {
        return true;
    }
    
    // Xử lý lỗi (Logging có thể thêm ở đây) 
    return false;
}

bool bkit_receive_message(sensor_data_t* data) {
    if (data == NULL) return false;

    BKITSensorData proto_msg = BKITSensorData_init_zero;

    /* Bước 1: Gọi lớp Message Processing để nhận, check CRC và decode [cite: 127-130] */
    // Lưu ý: Hàm bkit_process_receive cần trả về BKIT_COM_OK CHỈ KHI có gói tin trọn vẹn
    if (bkit_process_receive(&proto_msg) == BKIT_COM_OK) {
        
        /* Bước 2: Chuyển đổi ngược từ Protobuf Struct về App Struct */
        data->sensor_id = proto_msg.sensor_id;
        data->value     = proto_msg.sensor_value;
        data->timestamp = proto_msg.timestamp;
        
        return true; // Báo cho người dùng biết đã có dữ liệu mới
    }

    return false; // Chưa có dữ liệu hoặc lỗi
}