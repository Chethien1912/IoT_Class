# DS3231 RTC Driver Example

## Mô tả
Driver cho DS3231 Real-Time Clock module sử dụng giao tiếp I2C trên ESP32.

## Tính năng
- ✅ Đọc và ghi thời gian/ngày tháng
- ✅ Đọc nhiệt độ từ cảm biến tích hợp
- ✅ Chuyển đổi BCD ↔ Decimal
- ✅ Kiểm tra lỗi oscillator
- ✅ Hỗ trợ định dạng 24 giờ

## Kết nối phần cứng

| DS3231 Pin | ESP32 Pin |
|------------|-----------|
| VCC        | 3.3V      |
| GND        | GND       |
| SDA        | GPIO 21   |
| SCL        | GPIO 22   |

## Cách sử dụng

### 1. Khởi tạo DS3231

```c
#include "sensor.h"

void app_main(void) {
    // Cấu hình DS3231
    ds3231_config_t config = {
        .i2c_port = I2C_NUM_0,
        .sda_pin = GPIO_NUM_21,
        .scl_pin = GPIO_NUM_22,
        .clk_speed = 100000  // 100kHz
    };
    
    // Khởi tạo
    esp_err_t ret = ds3231_init(&config);
    if (ret != ESP_OK) {
        printf("Lỗi khởi tạo DS3231!\n");
        return;
    }
    printf("DS3231 đã sẵn sàng!\n");
}
```

### 2. Đặt thời gian

```c
void set_current_time(void) {
    ds3231_time_t time = {
        .year = 2026,
        .month = 2,      // Tháng 2
        .date = 11,      // Ngày 11
        .day = 3,        // Thứ 3 (1=CN, 2=T2, ..., 7=T7)
        .hours = 9,      // 9 giờ
        .minutes = 52,   // 52 phút
        .seconds = 0     // 0 giây
    };
    
    esp_err_t ret = ds3231_set_time(&time);
    if (ret == ESP_OK) {
        printf("Đã đặt thời gian thành công!\n");
    }
}
```

### 3. Đọc thời gian

```c
void read_current_time(void) {
    ds3231_time_t time;
    
    esp_err_t ret = ds3231_get_time(&time);
    if (ret == ESP_OK) {
        printf("Thời gian: %04d-%02d-%02d %02d:%02d:%02d\n",
               time.year, time.month, time.date,
               time.hours, time.minutes, time.seconds);
    }
}
```

### 4. Đọc nhiệt độ

```c
void read_temperature(void) {
    float temperature;
    
    esp_err_t ret = ds3231_get_temperature(&temperature);
    if (ret == ESP_OK) {
        printf("Nhiệt độ: %.2f°C\n", temperature);
    }
}
```

### 5. Ví dụ hoàn chỉnh

```c
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sensor.h"

void app_main(void) {
    // Khởi tạo DS3231
    ds3231_config_t config = {
        .i2c_port = I2C_NUM_0,
        .sda_pin = GPIO_NUM_21,
        .scl_pin = GPIO_NUM_22,
        .clk_speed = 100000
    };
    
    if (ds3231_init(&config) != ESP_OK) {
        printf("Lỗi khởi tạo DS3231!\n");
        return;
    }
    
    // Đặt thời gian ban đầu (chỉ cần làm 1 lần)
    ds3231_time_t init_time = {
        .year = 2026,
        .month = 2,
        .date = 11,
        .day = 3,
        .hours = 9,
        .minutes = 52,
        .seconds = 0
    };
    ds3231_set_time(&init_time);
    
    // Vòng lặp đọc thời gian và nhiệt độ
    while (1) {
        ds3231_time_t time;
        float temperature;
        
        // Đọc thời gian
        if (ds3231_get_time(&time) == ESP_OK) {
            printf("📅 %04d-%02d-%02d (Thứ %d) %02d:%02d:%02d",
                   time.year, time.month, time.date, time.day,
                   time.hours, time.minutes, time.seconds);
        }
        
        // Đọc nhiệt độ
        if (ds3231_get_temperature(&temperature) == ESP_OK) {
            printf(" | 🌡️  %.2f°C\n", temperature);
        }
        
        vTaskDelay(pdMS_TO_TICKS(1000));  // Delay 1 giây
    }
}
```

## API Reference

### Cấu trúc dữ liệu

#### `ds3231_config_t`
```c
typedef struct {
    i2c_port_t i2c_port;    // Cổng I2C (I2C_NUM_0 hoặc I2C_NUM_1)
    gpio_num_t sda_pin;     // Chân SDA
    gpio_num_t scl_pin;     // Chân SCL
    uint32_t clk_speed;     // Tốc độ I2C (Hz), thường là 100000
} ds3231_config_t;
```

#### `ds3231_time_t`
```c
typedef struct {
    uint8_t seconds;    // Giây (0-59)
    uint8_t minutes;    // Phút (0-59)
    uint8_t hours;      // Giờ (0-23, định dạng 24h)
    uint8_t day;        // Thứ (1-7: 1=CN, 2=T2, ..., 7=T7)
    uint8_t date;       // Ngày (1-31)
    uint8_t month;      // Tháng (1-12)
    uint16_t year;      // Năm (2000-2099)
} ds3231_time_t;
```

### Hàm API

#### `ds3231_init()`
Khởi tạo DS3231 và I2C driver.
```c
esp_err_t ds3231_init(const ds3231_config_t *config);
```

#### `ds3231_set_time()`
Đặt thời gian cho DS3231.
```c
esp_err_t ds3231_set_time(const ds3231_time_t *time);
```

#### `ds3231_get_time()`
Đọc thời gian từ DS3231.
```c
esp_err_t ds3231_get_time(ds3231_time_t *time);
```

#### `ds3231_get_temperature()`
Đọc nhiệt độ từ cảm biến tích hợp.
```c
esp_err_t ds3231_get_temperature(float *temperature);
```

#### `ds3231_deinit()`
Hủy khởi tạo DS3231 và I2C driver.
```c
esp_err_t ds3231_deinit(void);
```

## Lưu ý

1. **Nguồn điện**: DS3231 hoạt động ở 3.3V, phù hợp với ESP32
2. **Pull-up resistor**: Module DS3231 thường đã có điện trở kéo lên tích hợp
3. **Battery backup**: DS3231 có thể sử dụng pin CR2032 để giữ thời gian khi mất nguồn
4. **Độ chính xác**: ±2ppm (±1 phút/năm) ở 0°C đến +40°C
5. **Nhiệt độ**: Cảm biến nhiệt độ có độ chính xác ±3°C

## Troubleshooting

### Lỗi I2C Communication
```
E (xxx) DS3231: Failed to read from register 0x00
```
**Giải pháp:**
- Kiểm tra kết nối SDA/SCL
- Kiểm tra nguồn điện 3.3V
- Thử giảm tốc độ I2C xuống 50000 Hz

### Oscillator Stopped
```
W (xxx) DS3231: Oscillator was stopped, clearing flag
```
**Giải pháp:**
- Đặt lại thời gian bằng `ds3231_set_time()`
- Kiểm tra pin backup (nếu có)

### Thời gian không chính xác
**Giải pháp:**
- Đặt lại thời gian chính xác
- Kiểm tra pin backup đã hết chưa
