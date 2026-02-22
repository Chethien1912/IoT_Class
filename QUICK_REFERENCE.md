# ⚡ ESP32 Quick Reference - Tham khảo nhanh

## 🔥 Lệnh hay dùng nhất

```bash
# Build, flash và xem log (dùng nhiều nhất!)
idf.py build flash monitor

# Chỉ flash và xem log (khi đã build)
idf.py flash monitor

# Thoát monitor
Ctrl + ]

# Chọn port COM
idf.py -p COM3 flash monitor

# Xóa build và build lại
idf.py fullclean
idf.py build
```

---

## 📁 Cấu trúc file cơ bản

### Header file (.h)
```c
#ifndef SENSOR_H
#define SENSOR_H

#include "esp_err.h"

// Khai báo hàm
esp_err_t sensor_init(void);
esp_err_t sensor_read(float *value);

#endif
```

### Implementation file (.c)
```c
#include "sensor.h"
#include "esp_log.h"

static const char *TAG = "SENSOR";

esp_err_t sensor_init(void) {
    ESP_LOGI(TAG, "Init OK");
    return ESP_OK;
}

esp_err_t sensor_read(float *value) {
    if (value == NULL) return ESP_ERR_INVALID_ARG;
    *value = 25.5;
    return ESP_OK;
}
```

### main.c
```c
#include "sensor.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void) {
    sensor_init();
    
    while (1) {
        float temp;
        sensor_read(&temp);
        printf("Temp: %.2f\n", temp);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

---

## 🔧 CMakeLists.txt

### main/CMakeLists.txt
```cmake
idf_component_register(
    SRCS "main.c"
    REQUIRES Sensor Relay
)
```

### components/Sensor/CMakeLists.txt
```cmake
idf_component_register(
    SRCS "sensor.c"
    INCLUDE_DIRS "include"
    REQUIRES driver
)
```

---

## 📝 Code patterns thường dùng

### 1. Kiểm tra lỗi
```c
esp_err_t ret = sensor_init();
if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Error: %s", esp_err_to_name(ret));
    return ret;
}
```

### 2. Logging
```c
ESP_LOGE(TAG, "Error");      // Đỏ - Lỗi
ESP_LOGW(TAG, "Warning");    // Vàng - Cảnh báo
ESP_LOGI(TAG, "Info");       // Xanh - Thông tin
ESP_LOGD(TAG, "Debug");      // Xám - Debug
```

### 3. Delay
```c
vTaskDelay(pdMS_TO_TICKS(1000));  // Delay 1 giây
vTaskDelay(pdMS_TO_TICKS(500));   // Delay 0.5 giây
```

### 4. GPIO
```c
#include "driver/gpio.h"

// Output
gpio_set_direction(GPIO_NUM_25, GPIO_MODE_OUTPUT);
gpio_set_level(GPIO_NUM_25, 1);  // HIGH
gpio_set_level(GPIO_NUM_25, 0);  // LOW

// Input
gpio_set_direction(GPIO_NUM_26, GPIO_MODE_INPUT);
int level = gpio_get_level(GPIO_NUM_26);
```

### 5. Task
```c
void my_task(void *param) {
    while (1) {
        // Do something
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void) {
    xTaskCreate(my_task, "my_task", 4096, NULL, 5, NULL);
}
```

### 6. I2C
```c
#include "driver/i2c.h"

// Init
i2c_config_t conf = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = GPIO_NUM_21,
    .scl_io_num = GPIO_NUM_22,
    .master.clk_speed = 100000,
};
i2c_param_config(I2C_NUM_0, &conf);
i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);

// Write
uint8_t data[] = {0x01, 0x02};
i2c_master_write_to_device(I2C_NUM_0, 0x68, data, 2, 1000/portTICK_PERIOD_MS);

// Read
uint8_t buf[10];
i2c_master_read_from_device(I2C_NUM_0, 0x68, buf, 10, 1000/portTICK_PERIOD_MS);
```

---

## ⚠️ Lỗi thường gặp

### 1. "Component not found"
**Nguyên nhân**: Chưa thêm component vào REQUIRES
```cmake
# Sửa trong main/CMakeLists.txt
idf_component_register(
    SRCS "main.c"
    REQUIRES Sensor  # ← Thêm dòng này
)
```

### 2. "Undefined reference"
**Nguyên nhân**: Chưa include header hoặc chưa link component
```c
#include "sensor.h"  // ← Thêm include
```

### 3. "File not found"
**Nguyên nhân**: Sai đường dẫn INCLUDE_DIRS
```cmake
idf_component_register(
    INCLUDE_DIRS "include"  # ← Kiểm tra đường dẫn
)
```

### 4. Crash khi chạy
**Nguyên nhân**: Không kiểm tra NULL pointer
```c
// ❌ SAI
esp_err_t read(float *val) {
    *val = 10;  // Crash nếu val = NULL
}

// ✅ ĐÚNG
esp_err_t read(float *val) {
    if (val == NULL) return ESP_ERR_INVALID_ARG;
    *val = 10;
    return ESP_OK;
}
```

---

## 🎯 Checklist tạo component mới

- [ ] Tạo thư mục `components/MyComponent/`
- [ ] Tạo thư mục `components/MyComponent/include/`
- [ ] Tạo file `mycomponent.h` trong `include/`
- [ ] Tạo file `mycomponent.c`
- [ ] Tạo file `CMakeLists.txt`
- [ ] Thêm component vào `main/CMakeLists.txt` (REQUIRES)
- [ ] Include header trong `main.c`
- [ ] Build: `idf.py build`

---

## 💡 Tips

1. **Luôn kiểm tra return value**: `if (ret != ESP_OK)`
2. **Luôn kiểm tra NULL pointer**: `if (ptr == NULL)`
3. **Dùng ESP_LOG thay vì printf**: Có thể filter level
4. **Dùng vTaskDelay thay vì busy-wait**: Tiết kiệm điện
5. **Đặt tên rõ ràng**: `sensor_init()` thay vì `init()`
6. **Thêm TAG cho mỗi module**: `static const char *TAG = "MODULE";`

---

## 🔗 Link hữu ích

- [ESP-IDF Docs](https://docs.espressif.com/projects/esp-idf/en/latest/)
- [API Reference](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/index.html)
- [Examples](https://github.com/espressif/esp-idf/tree/master/examples)
