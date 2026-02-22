# Hướng dẫn sử dụng thư viện WifiConnection

Thư viện này giúp ESP32 kết nối Wifi dễ dàng, hỗ trợ tự động kết nối lại khi mất mạng.

## 1. Cấu hình Wifi (SSID & Password)

Bạn không cần sửa code để đổi Wifi! Hãy dùng `menuconfig`:

1. Mở terminal, chạy: `idf.py menuconfig`
2. Vào menu: **Component config** -> **Wifi Configuration**
3. Nhập **SSID** và **Password** của bạn.
4. Nhấn `S` để Save và `Esc` để thoát.

## 2. Cách sử dụng trong code (main.c)

```c
#include "wifi_connection.h"

void app_main(void) {
    // 1. Khởi tạo Wifi Station
    wifi_init_sta();
    
    // 2. Chờ kết nối (Hàm này sẽ block cho đến khi kết nối được)
    if (wifi_wait_for_connection()) {
        printf("Kết nối thành công!\n");
    } else {
        printf("Kết nối thất bại!\n");
    }
    
    // 3. Sau khi kết nối, chương trình tiếp tục chạy các task khác...
}
```

## 3. Các tính năng

- **Auto Reconnect**: Nếu mất mạng, thư viện sẽ tự động thử kết nối lại (mặc định 5 lần, có thể chỉnh trong menuconfig).
- **Event Group**: Sử dụng FreeRTOS Event Group để đồng bộ hóa trạng thái kết nối.
- **NVS Flash**: Tự động khởi tạo NVS (nơi lưu info Wifi).

## 4. API Reference

| Hàm | Mô tả |
|-----|-------|
| `wifi_init_sta()` | Khởi tạo chế độ Station (kết nối Router) |
| `wifi_init_ap()` | Khởi tạo chế độ Access Point (Phát Wifi - *chưa hoàn thiện*) |
| `wifi_wait_for_connection()` | Chờ cho đến khi kết nối thành công (trả về true/false) |

---
**Lưu ý**: Để sử dụng thư viện này, file `CMakeLists.txt` của bạn phải có `REQUIRES WifiConnection`.
