# 📚 Hướng Dẫn Lập Trình ESP32 với ESP-IDF

## 📖 Mục lục
1. [Cấu trúc dự án ESP-IDF](#cấu-trúc-dự-án-esp-idf)
2. [Các lệnh thông dụng](#các-lệnh-thông-dụng)
3. [Cấu trúc code cơ bản](#cấu-trúc-code-cơ-bản)
4. [CMakeLists.txt](#cmakelists)
5. [Kconfig](#kconfig)
6. [Các khái niệm quan trọng](#các-khái-niệm-quan-trọng)
7. [Best Practices](#best-practices)

---

## 🏗️ Cấu trúc dự án ESP-IDF

```
IoTClass/
├── CMakeLists.txt              # File cấu hình build chính
├── sdkconfig                   # File cấu hình được tạo từ menuconfig
├── main/                       # Thư mục chứa code chính
│   ├── CMakeLists.txt         # Cấu hình build cho main
│   └── main.c                 # File chương trình chính
├── components/                 # Thư mục chứa các component tự tạo
│   ├── Sensor/
│   │   ├── CMakeLists.txt     # Cấu hình build cho component
│   │   ├── Kconfig.projbuild  # Cấu hình menuconfig (tùy chọn)
│   │   ├── include/           # Thư mục chứa header files
│   │   │   └── sensor.h
│   │   └── sensor.c           # File implementation
│   └── Relay/
│       ├── CMakeLists.txt
│       ├── include/
│       │   └── Relay.h
│       └── Relay.c
└── build/                      # Thư mục build (tự động tạo)
```

### 📝 Giải thích cấu trúc:

- **main/**: Chứa code chính của ứng dụng, điểm bắt đầu là hàm `app_main()`
- **components/**: Chứa các thư viện/module tự tạo (như Sensor, Relay)
- **include/**: Chứa các file header (.h) để khai báo hàm, struct
- **CMakeLists.txt**: File cấu hình để build project
- **sdkconfig**: File cấu hình các tùy chọn của ESP-IDF

---

## ⚡ Các lệnh thông dụng

### 1. Build và Flash

```bash
# Build project (biên dịch code)
idf.py build

# Flash code lên ESP32 (nạp chương trình)
idf.py flash

# Mở Serial Monitor để xem log
idf.py monitor

# Build + Flash + Monitor (làm tất cả cùng lúc)
idf.py build flash monitor

# Flash và monitor (không build lại)
idf.py flash monitor
```

### 2. Cấu hình

```bash
# Mở menu cấu hình (menuconfig)
idf.py menuconfig

# Xóa cấu hình và build lại từ đầu
idf.py fullclean

# Xóa build folder
idf.py clean
```

### 3. Port và Baudrate

```bash
# Chỉ định port COM (Windows)
idf.py -p COM3 flash monitor

# Chỉ định port (Linux/Mac)
idf.py -p /dev/ttyUSB0 flash monitor

# Thay đổi baudrate cho monitor
idf.py monitor -b 115200
```

### 4. Lệnh khác

```bash
# Xem kích thước binary
idf.py size

# Xem kích thước chi tiết theo component
idf.py size-components

# Tạo project mới
idf.py create-project my_project

# Tạo component mới
idf.py create-component my_component
```

### 5. Thoát Monitor

- **Windows/Linux/Mac**: `Ctrl + ]`

---

## 💻 Cấu trúc code cơ bản

### 1. File Header (.h)

**Mục đích**: Khai báo các hàm, struct, define để các file khác sử dụng

```c
#ifndef SENSOR_H          // Header guard (tránh include trùng)
#define SENSOR_H

#include <stdint.h>       // Thư viện chuẩn C
#include "esp_err.h"      // Thư viện ESP-IDF

// Định nghĩa hằng số
#define SENSOR_MAX_VALUE 100

// Định nghĩa struct
typedef struct {
    uint8_t id;
    float value;
} sensor_data_t;

// Khai báo hàm (function prototype)
esp_err_t sensor_init(void);
esp_err_t sensor_read(sensor_data_t *data);

#endif // SENSOR_H
```

### 2. File Implementation (.c)

**Mục đích**: Viết code thực thi các hàm đã khai báo

```c
#include "sensor.h"       // Include header của chính nó
#include "esp_log.h"      // Thư viện log
#include <string.h>       // Thư viện string

static const char *TAG = "SENSOR";  // Tag cho logging

// Biến static (chỉ dùng trong file này)
static bool is_initialized = false;

// Implementation của hàm
esp_err_t sensor_init(void) {
    if (is_initialized) {
        ESP_LOGW(TAG, "Already initialized");
        return ESP_OK;
    }
    
    // Code khởi tạo sensor
    ESP_LOGI(TAG, "Sensor initialized");
    is_initialized = true;
    
    return ESP_OK;
}

esp_err_t sensor_read(sensor_data_t *data) {
    if (!is_initialized) {
        ESP_LOGE(TAG, "Not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    
    if (data == NULL) {
        ESP_LOGE(TAG, "Data pointer is NULL");
        return ESP_ERR_INVALID_ARG;
    }
    
    // Code đọc sensor
    data->id = 1;
    data->value = 25.5;
    
    ESP_LOGD(TAG, "Read sensor: %.2f", data->value);
    return ESP_OK;
}
```

### 3. File main.c

**Mục đích**: Điểm bắt đầu của chương trình

```c
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sensor.h"           // Include component của bạn

static const char *TAG = "MAIN";

void app_main(void) {
    ESP_LOGI(TAG, "Starting application...");
    
    // 1. Khởi tạo các module
    esp_err_t ret = sensor_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init sensor");
        return;
    }
    
    // 2. Vòng lặp chính
    while (1) {
        sensor_data_t data;
        
        // Đọc sensor
        if (sensor_read(&data) == ESP_OK) {
            printf("Sensor value: %.2f\n", data.value);
        }
        
        // Delay 1 giây
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

---

## 🔧 CMakeLists.txt

### 1. CMakeLists.txt chính (root)

```cmake
cmake_minimum_required(VERSION 3.16)

# Tên project
project(IoTClass)

# Include ESP-IDF build system
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
```

### 2. CMakeLists.txt trong main/

```cmake
idf_component_register(
    SRCS "main.c"                    # File source
    INCLUDE_DIRS "."                 # Thư mục include
    REQUIRES Sensor Relay            # Component phụ thuộc
)
```

### 3. CMakeLists.txt trong component/

```cmake
idf_component_register(
    SRCS "sensor.c"                  # File source
    INCLUDE_DIRS "include"           # Thư mục chứa header
    REQUIRES driver                  # ESP-IDF component phụ thuộc
)
```

**Giải thích:**
- `SRCS`: Danh sách file .c cần biên dịch
- `INCLUDE_DIRS`: Thư mục chứa file .h
- `REQUIRES`: Component khác mà component này cần dùng
- `PRIV_REQUIRES`: Component chỉ dùng trong implementation (không public)

---

## ⚙️ Kconfig

**Mục đích**: Tạo menu cấu hình trong `idf.py menuconfig`

### File: components/Relay/Kconfig.projbuild

```kconfig
menu "Relay Configuration"

    config RELAY_1_ENABLE
        bool "Enable Relay 1"
        default y
        help
            Enable or disable Relay 1

    config RELAY_1_PIN
        int "Relay 1 GPIO Pin"
        depends on RELAY_1_ENABLE
        default 25
        help
            GPIO pin for Relay 1

    config RELAY_1_ACTIVE_HIGH
        bool "Relay 1 Active High"
        depends on RELAY_1_ENABLE
        default y
        help
            Set to true if relay is active high

endmenu
```

### Sử dụng trong code:

```c
#ifdef CONFIG_RELAY_1_ENABLE
    relay_init(0, CONFIG_RELAY_1_PIN, CONFIG_RELAY_1_ACTIVE_HIGH);
#endif
```

---

## 🎯 Các khái niệm quan trọng

### 1. ESP Error Handling

```c
// Kiểu trả về chuẩn của ESP-IDF
esp_err_t my_function(void) {
    // Thành công
    return ESP_OK;
    
    // Lỗi tham số không hợp lệ
    return ESP_ERR_INVALID_ARG;
    
    // Lỗi trạng thái không hợp lệ
    return ESP_ERR_INVALID_STATE;
    
    // Lỗi timeout
    return ESP_ERR_TIMEOUT;
}

// Kiểm tra lỗi
esp_err_t ret = my_function();
if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Error: %s", esp_err_to_name(ret));
}
```

### 2. Logging

```c
#include "esp_log.h"

static const char *TAG = "MY_MODULE";

// Các mức độ log (từ thấp đến cao)
ESP_LOGV(TAG, "Verbose");    // Chi tiết nhất
ESP_LOGD(TAG, "Debug");      // Debug
ESP_LOGI(TAG, "Info");       // Thông tin
ESP_LOGW(TAG, "Warning");    // Cảnh báo
ESP_LOGE(TAG, "Error");      // Lỗi

// Log với format
ESP_LOGI(TAG, "Temperature: %.2f°C", temp);
```

### 3. FreeRTOS Task

```c
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Hàm task
void my_task(void *pvParameters) {
    while (1) {
        printf("Task running\n");
        vTaskDelay(pdMS_TO_TICKS(1000));  // Delay 1 giây
    }
}

// Tạo task trong app_main()
void app_main(void) {
    xTaskCreate(
        my_task,           // Hàm task
        "my_task",         // Tên task
        4096,              // Stack size (bytes)
        NULL,              // Tham số truyền vào
        5,                 // Priority (0-25, cao hơn = ưu tiên hơn)
        NULL               // Task handle
    );
}
```

### 4. GPIO

```c
#include "driver/gpio.h"

// Cấu hình GPIO output
gpio_config_t io_conf = {
    .pin_bit_mask = (1ULL << GPIO_NUM_25),
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE
};
gpio_config(&io_conf);

// Bật/tắt GPIO
gpio_set_level(GPIO_NUM_25, 1);  // HIGH
gpio_set_level(GPIO_NUM_25, 0);  // LOW

// Đọc GPIO input
int level = gpio_get_level(GPIO_NUM_26);
```

### 5. I2C

```c
#include "driver/i2c.h"

// Cấu hình I2C
i2c_config_t conf = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = GPIO_NUM_21,
    .scl_io_num = GPIO_NUM_22,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master.clk_speed = 100000,
};

i2c_param_config(I2C_NUM_0, &conf);
i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);

// Ghi dữ liệu
uint8_t data[] = {0x01, 0x02};
i2c_master_write_to_device(I2C_NUM_0, 0x68, data, 2, 1000 / portTICK_PERIOD_MS);

// Đọc dữ liệu
uint8_t buffer[10];
i2c_master_read_from_device(I2C_NUM_0, 0x68, buffer, 10, 1000 / portTICK_PERIOD_MS);
```

---

## ✅ Best Practices

### 1. Cấu trúc code

```c
// ✅ ĐÚNG: Kiểm tra tham số
esp_err_t sensor_read(sensor_data_t *data) {
    if (data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    // ... code
}

// ❌ SAI: Không kiểm tra
esp_err_t sensor_read(sensor_data_t *data) {
    data->value = 10;  // Crash nếu data = NULL!
}
```

### 2. Error Handling

```c
// ✅ ĐÚNG: Kiểm tra lỗi
esp_err_t ret = sensor_init();
if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Init failed: %s", esp_err_to_name(ret));
    return ret;
}

// ❌ SAI: Không kiểm tra
sensor_init();  // Không biết có lỗi hay không
```

### 3. Logging

```c
// ✅ ĐÚNG: Sử dụng ESP_LOG
ESP_LOGI(TAG, "Temperature: %.2f", temp);

// ❌ SAI: Dùng printf (không có mức độ, không filter được)
printf("Temperature: %.2f\n", temp);
```

### 4. Delay

```c
// ✅ ĐÚNG: Dùng vTaskDelay (không block CPU)
vTaskDelay(pdMS_TO_TICKS(1000));

// ❌ SAI: Dùng busy-wait (lãng phí CPU)
for (int i = 0; i < 1000000; i++);
```

### 5. Header Guard

```c
// ✅ ĐÚNG: Có header guard
#ifndef SENSOR_H
#define SENSOR_H
// ... code
#endif

// ❌ SAI: Không có header guard (có thể include trùng)
// ... code
```

---

## 🚀 Workflow cơ bản

### Tạo component mới

1. **Tạo thư mục component**
   ```bash
   mkdir -p components/MyComponent/include
   ```

2. **Tạo file header** (`components/MyComponent/include/mycomponent.h`)
   ```c
   #ifndef MYCOMPONENT_H
   #define MYCOMPONENT_H
   
   #include "esp_err.h"
   
   esp_err_t mycomponent_init(void);
   
   #endif
   ```

3. **Tạo file implementation** (`components/MyComponent/mycomponent.c`)
   ```c
   #include "mycomponent.h"
   #include "esp_log.h"
   
   static const char *TAG = "MYCOMPONENT";
   
   esp_err_t mycomponent_init(void) {
       ESP_LOGI(TAG, "Initialized");
       return ESP_OK;
   }
   ```

4. **Tạo CMakeLists.txt** (`components/MyComponent/CMakeLists.txt`)
   ```cmake
   idf_component_register(
       SRCS "mycomponent.c"
       INCLUDE_DIRS "include"
   )
   ```

5. **Sử dụng trong main.c**
   ```c
   #include "mycomponent.h"
   
   void app_main(void) {
       mycomponent_init();
   }
   ```

6. **Cập nhật main/CMakeLists.txt**
   ```cmake
   idf_component_register(
       SRCS "main.c"
       REQUIRES MyComponent
   )
   ```

7. **Build và flash**
   ```bash
   idf.py build flash monitor
   ```

---

## 📌 Tóm tắt nhanh

| Thao tác | Lệnh |
|----------|------|
| Build | `idf.py build` |
| Flash | `idf.py flash` |
| Monitor | `idf.py monitor` |
| Tất cả | `idf.py build flash monitor` |
| Cấu hình | `idf.py menuconfig` |
| Clean | `idf.py fullclean` |
| Chọn port | `idf.py -p COM3 flash` |
| Thoát monitor | `Ctrl + ]` |

---

## 🔗 Tài liệu tham khảo

- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/)
- [ESP-IDF API Reference](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/index.html)
- [FreeRTOS Documentation](https://www.freertos.org/Documentation/RTOS_book.html)

---

**Chúc bạn học tập và phát triển thành công với ESP32! 🎉**
