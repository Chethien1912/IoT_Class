# 📖 Hướng Dẫn Các Hàm Thông Dụng Trong C/C++ cho ESP32

## 📑 Mục lục
1. [String Functions (Xử lý chuỗi)](#string-functions)
2. [Memory Functions (Xử lý bộ nhớ)](#memory-functions)
3. [Math Functions (Toán học)](#math-functions)
4. [Time Functions (Thời gian)](#time-functions)
5. [Standard I/O (Nhập xuất)](#standard-io)
6. [ESP-IDF Specific Functions](#esp-idf-functions)
7. [FreeRTOS Functions](#freertos-functions)
8. [GPIO Functions](#gpio-functions)
9. [I2C Functions](#i2c-functions)
10. [SPI Functions](#spi-functions)
11. [UART Functions](#uart-functions)

---

## 📝 String Functions (Xử lý chuỗi)

### `#include <string.h>`

#### 1. `strlen()` - Đo độ dài chuỗi
```c
size_t strlen(const char *str);
```
**Công dụng**: Trả về độ dài chuỗi (không tính ký tự `\0`)

**Ví dụ**:
```c
char str[] = "Hello";
int len = strlen(str);  // len = 5
printf("Length: %d\n", len);
```

#### 2. `strcpy()` - Sao chép chuỗi
```c
char *strcpy(char *dest, const char *src);
```
**Công dụng**: Copy chuỗi từ `src` sang `dest`

**Ví dụ**:
```c
char src[] = "Hello";
char dest[20];
strcpy(dest, src);  // dest = "Hello"
printf("%s\n", dest);
```

⚠️ **Lưu ý**: `dest` phải đủ lớn để chứa `src`

#### 3. `strncpy()` - Sao chép n ký tự
```c
char *strncpy(char *dest, const char *src, size_t n);
```
**Công dụng**: Copy tối đa `n` ký tự từ `src` sang `dest`

**Ví dụ**:
```c
char src[] = "Hello World";
char dest[20];
strncpy(dest, src, 5);  // dest = "Hello"
dest[5] = '\0';  // Thêm null terminator
```

#### 4. `strcat()` - Nối chuỗi
```c
char *strcat(char *dest, const char *src);
```
**Công dụng**: Nối chuỗi `src` vào cuối `dest`

**Ví dụ**:
```c
char dest[20] = "Hello";
char src[] = " World";
strcat(dest, src);  // dest = "Hello World"
```

#### 5. `strcmp()` - So sánh chuỗi
```c
int strcmp(const char *str1, const char *str2);
```
**Công dụng**: So sánh 2 chuỗi
- Trả về `0` nếu bằng nhau
- Trả về `< 0` nếu str1 < str2
- Trả về `> 0` nếu str1 > str2

**Ví dụ**:
```c
if (strcmp("abc", "abc") == 0) {
    printf("Equal\n");
}

if (strcmp("abc", "xyz") < 0) {
    printf("abc < xyz\n");
}
```

#### 6. `strstr()` - Tìm chuỗi con
```c
char *strstr(const char *haystack, const char *needle);
```
**Công dụng**: Tìm chuỗi `needle` trong `haystack`

**Ví dụ**:
```c
char str[] = "Hello World";
char *pos = strstr(str, "World");
if (pos != NULL) {
    printf("Found at: %ld\n", pos - str);  // Found at: 6
}
```

#### 7. `sprintf()` - Format chuỗi
```c
int sprintf(char *str, const char *format, ...);
```
**Công dụng**: Ghi chuỗi đã format vào buffer

**Ví dụ**:
```c
char buffer[50];
int temp = 25;
sprintf(buffer, "Temperature: %d°C", temp);
printf("%s\n", buffer);  // Temperature: 25°C
```

#### 8. `snprintf()` - Format chuỗi an toàn
```c
int snprintf(char *str, size_t size, const char *format, ...);
```
**Công dụng**: Giống `sprintf` nhưng giới hạn kích thước

**Ví dụ**:
```c
char buffer[20];
snprintf(buffer, sizeof(buffer), "Temp: %d°C", 25);
```

#### 9. `sscanf()` - Parse chuỗi
```c
int sscanf(const char *str, const char *format, ...);
```
**Công dụng**: Đọc dữ liệu từ chuỗi theo format

**Ví dụ**:
```c
char str[] = "25 30";
int a, b;
sscanf(str, "%d %d", &a, &b);  // a=25, b=30
```

#### 10. `strtok()` - Tách chuỗi
```c
char *strtok(char *str, const char *delim);
```
**Công dụng**: Tách chuỗi theo delimiter

**Ví dụ**:
```c
char str[] = "one,two,three";
char *token = strtok(str, ",");
while (token != NULL) {
    printf("%s\n", token);
    token = strtok(NULL, ",");
}
// Output: one, two, three
```

---

## 💾 Memory Functions (Xử lý bộ nhớ)

### `#include <string.h>` và `#include <stdlib.h>`

#### 1. `memset()` - Gán giá trị cho vùng nhớ
```c
void *memset(void *ptr, int value, size_t num);
```
**Công dụng**: Gán `value` cho `num` bytes bắt đầu từ `ptr`

**Ví dụ**:
```c
char buffer[10];
memset(buffer, 0, sizeof(buffer));  // Gán tất cả = 0

int arr[5];
memset(arr, 0, sizeof(arr));  // Reset array về 0
```

#### 2. `memcpy()` - Copy vùng nhớ
```c
void *memcpy(void *dest, const void *src, size_t n);
```
**Công dụng**: Copy `n` bytes từ `src` sang `dest`

**Ví dụ**:
```c
uint8_t src[] = {1, 2, 3, 4, 5};
uint8_t dest[5];
memcpy(dest, src, sizeof(src));
```

#### 3. `memcmp()` - So sánh vùng nhớ
```c
int memcmp(const void *ptr1, const void *ptr2, size_t num);
```
**Công dụng**: So sánh `num` bytes của 2 vùng nhớ

**Ví dụ**:
```c
uint8_t arr1[] = {1, 2, 3};
uint8_t arr2[] = {1, 2, 3};
if (memcmp(arr1, arr2, 3) == 0) {
    printf("Equal\n");
}
```

#### 4. `malloc()` - Cấp phát bộ nhớ động
```c
void *malloc(size_t size);
```
**Công dụng**: Cấp phát `size` bytes bộ nhớ

**Ví dụ**:
```c
int *arr = (int *)malloc(5 * sizeof(int));
if (arr != NULL) {
    arr[0] = 10;
    arr[1] = 20;
    free(arr);  // Phải giải phóng sau khi dùng
}
```

#### 5. `calloc()` - Cấp phát và khởi tạo 0
```c
void *calloc(size_t num, size_t size);
```
**Công dụng**: Cấp phát bộ nhớ và khởi tạo = 0

**Ví dụ**:
```c
int *arr = (int *)calloc(5, sizeof(int));
// arr = {0, 0, 0, 0, 0}
free(arr);
```

#### 6. `realloc()` - Thay đổi kích thước bộ nhớ
```c
void *realloc(void *ptr, size_t size);
```
**Công dụng**: Thay đổi kích thước vùng nhớ đã cấp phát

**Ví dụ**:
```c
int *arr = (int *)malloc(5 * sizeof(int));
arr = (int *)realloc(arr, 10 * sizeof(int));  // Mở rộng lên 10
free(arr);
```

#### 7. `free()` - Giải phóng bộ nhớ
```c
void free(void *ptr);
```
**Công dụng**: Giải phóng bộ nhớ đã cấp phát

**Ví dụ**:
```c
int *arr = (int *)malloc(10 * sizeof(int));
// ... sử dụng arr
free(arr);
arr = NULL;  // Good practice
```

---

## 🔢 Math Functions (Toán học)

### `#include <math.h>`

#### 1. `abs()` / `fabs()` - Giá trị tuyệt đối
```c
int abs(int x);
double fabs(double x);
```
**Ví dụ**:
```c
int a = abs(-10);      // a = 10
double b = fabs(-3.5); // b = 3.5
```

#### 2. `pow()` - Lũy thừa
```c
double pow(double base, double exponent);
```
**Ví dụ**:
```c
double result = pow(2, 3);  // 2^3 = 8
double result2 = pow(5, 2); // 5^2 = 25
```

#### 3. `sqrt()` - Căn bậc 2
```c
double sqrt(double x);
```
**Ví dụ**:
```c
double result = sqrt(16);  // result = 4
double result2 = sqrt(2);  // result2 = 1.414...
```

#### 4. `ceil()` / `floor()` / `round()` - Làm tròn
```c
double ceil(double x);   // Làm tròn lên
double floor(double x);  // Làm tròn xuống
double round(double x);  // Làm tròn gần nhất
```
**Ví dụ**:
```c
double a = ceil(3.2);   // a = 4
double b = floor(3.8);  // b = 3
double c = round(3.5);  // c = 4
```

#### 5. `sin()` / `cos()` / `tan()` - Lượng giác
```c
double sin(double x);
double cos(double x);
double tan(double x);
```
**Ví dụ**:
```c
#include <math.h>
#define PI 3.14159265

double angle = 45 * PI / 180;  // Chuyển độ sang radian
double s = sin(angle);
double c = cos(angle);
```

#### 6. `log()` / `log10()` - Logarit
```c
double log(double x);    // ln(x)
double log10(double x);  // log10(x)
```
**Ví dụ**:
```c
double a = log(2.718);   // ln(e) ≈ 1
double b = log10(100);   // log10(100) = 2
```

#### 7. `min()` / `max()` - Giá trị nhỏ/lớn nhất
```c
// Macro trong ESP-IDF
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
```
**Ví dụ**:
```c
int a = MIN(10, 20);  // a = 10
int b = MAX(10, 20);  // b = 20
```

---

## ⏰ Time Functions (Thời gian)

### `#include <time.h>` và `#include <sys/time.h>`

#### 1. `time()` - Lấy thời gian hiện tại
```c
time_t time(time_t *timer);
```
**Ví dụ**:
```c
time_t now = time(NULL);
printf("Timestamp: %ld\n", now);
```

#### 2. `localtime()` - Chuyển timestamp sang struct tm
```c
struct tm *localtime(const time_t *timer);
```
**Ví dụ**:
```c
time_t now = time(NULL);
struct tm *timeinfo = localtime(&now);
printf("Year: %d\n", timeinfo->tm_year + 1900);
printf("Month: %d\n", timeinfo->tm_mon + 1);
printf("Day: %d\n", timeinfo->tm_mday);
printf("Hour: %d\n", timeinfo->tm_hour);
```

#### 3. `strftime()` - Format thời gian
```c
size_t strftime(char *str, size_t maxsize, const char *format, const struct tm *timeptr);
```
**Ví dụ**:
```c
time_t now = time(NULL);
struct tm *timeinfo = localtime(&now);
char buffer[80];
strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
printf("%s\n", buffer);  // 2026-02-11 10:30:00
```

#### 4. `gettimeofday()` - Lấy thời gian với microsecond
```c
int gettimeofday(struct timeval *tv, struct timezone *tz);
```
**Ví dụ**:
```c
struct timeval tv;
gettimeofday(&tv, NULL);
printf("Seconds: %ld\n", tv.tv_sec);
printf("Microseconds: %ld\n", tv.tv_usec);
```

---

## 📤 Standard I/O (Nhập xuất)

### `#include <stdio.h>`

#### 1. `printf()` - In ra console
```c
int printf(const char *format, ...);
```
**Format specifiers**:
- `%d` - Integer
- `%u` - Unsigned integer
- `%f` - Float
- `%.2f` - Float với 2 chữ số thập phân
- `%s` - String
- `%c` - Character
- `%x` - Hexadecimal
- `%p` - Pointer

**Ví dụ**:
```c
int age = 25;
float temp = 25.5;
char name[] = "ESP32";

printf("Age: %d\n", age);
printf("Temp: %.2f°C\n", temp);
printf("Name: %s\n", name);
printf("Hex: 0x%02X\n", 255);  // 0xFF
```

#### 2. `sprintf()` / `snprintf()` - Format vào string
```c
int sprintf(char *str, const char *format, ...);
int snprintf(char *str, size_t size, const char *format, ...);
```
**Ví dụ**:
```c
char buffer[50];
int temp = 25;
snprintf(buffer, sizeof(buffer), "Temp: %d°C", temp);
```

#### 3. `scanf()` / `sscanf()` - Đọc input
```c
int scanf(const char *format, ...);
int sscanf(const char *str, const char *format, ...);
```
**Ví dụ**:
```c
// Parse từ string
char str[] = "25 30";
int a, b;
sscanf(str, "%d %d", &a, &b);
```

---

## 🔧 ESP-IDF Specific Functions

### 1. Logging - `#include "esp_log.h"`

```c
ESP_LOGE(TAG, "Error message");      // Error (đỏ)
ESP_LOGW(TAG, "Warning message");    // Warning (vàng)
ESP_LOGI(TAG, "Info message");       // Info (xanh)
ESP_LOGD(TAG, "Debug message");      // Debug
ESP_LOGV(TAG, "Verbose message");    // Verbose

// Với format
ESP_LOGI(TAG, "Temperature: %.2f°C", temp);
ESP_LOGE(TAG, "Error code: %d", err);
```

### 2. Error Handling - `#include "esp_err.h"`

```c
// Kiểm tra lỗi
esp_err_t ret = some_function();
if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Error: %s", esp_err_to_name(ret));
}

// Các mã lỗi thường dùng
ESP_OK                  // Thành công
ESP_FAIL                // Lỗi chung
ESP_ERR_NO_MEM          // Hết bộ nhớ
ESP_ERR_INVALID_ARG     // Tham số không hợp lệ
ESP_ERR_INVALID_STATE   // Trạng thái không hợp lệ
ESP_ERR_TIMEOUT         // Timeout
ESP_ERR_NOT_FOUND       // Không tìm thấy
```

### 3. System Functions - `#include "esp_system.h"`

```c
// Reset ESP32
esp_restart();

// Lấy thông tin chip
esp_chip_info_t chip_info;
esp_chip_info(&chip_info);
printf("Cores: %d\n", chip_info.cores);

// Lấy free heap
uint32_t free_heap = esp_get_free_heap_size();
printf("Free heap: %lu bytes\n", free_heap);

// Lấy MAC address
uint8_t mac[6];
esp_read_mac(mac, ESP_MAC_WIFI_STA);
printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
       mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
```

---

## 🔄 FreeRTOS Functions

### `#include "freertos/FreeRTOS.h"` và `#include "freertos/task.h"`

#### 1. `vTaskDelay()` - Delay task
```c
void vTaskDelay(const TickType_t xTicksToDelay);
```
**Ví dụ**:
```c
vTaskDelay(pdMS_TO_TICKS(1000));  // Delay 1 giây
vTaskDelay(pdMS_TO_TICKS(500));   // Delay 0.5 giây
vTaskDelay(100 / portTICK_PERIOD_MS);  // Delay 100ms
```

#### 2. `xTaskCreate()` - Tạo task
```c
BaseType_t xTaskCreate(
    TaskFunction_t pvTaskCode,
    const char * const pcName,
    uint32_t usStackDepth,
    void *pvParameters,
    UBaseType_t uxPriority,
    TaskHandle_t *pxCreatedTask
);
```
**Ví dụ**:
```c
void my_task(void *param) {
    while (1) {
        printf("Task running\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void) {
    xTaskCreate(my_task, "my_task", 4096, NULL, 5, NULL);
}
```

#### 3. `xTaskCreatePinnedToCore()` - Tạo task trên core cụ thể
```c
xTaskCreatePinnedToCore(
    my_task,      // Function
    "my_task",    // Name
    4096,         // Stack size
    NULL,         // Parameters
    5,            // Priority
    NULL,         // Task handle
    0             // Core ID (0 hoặc 1)
);
```

#### 4. `vTaskDelete()` - Xóa task
```c
void vTaskDelete(TaskHandle_t xTask);
```
**Ví dụ**:
```c
vTaskDelete(NULL);  // Xóa task hiện tại
```

#### 5. `xTaskGetTickCount()` - Lấy tick count
```c
TickType_t xTaskGetTickCount(void);
```
**Ví dụ**:
```c
TickType_t start = xTaskGetTickCount();
// Do something
TickType_t end = xTaskGetTickCount();
printf("Time: %lu ms\n", (end - start) * portTICK_PERIOD_MS);
```

---

## 📌 GPIO Functions

### `#include "driver/gpio.h"`

#### 1. `gpio_set_direction()` - Đặt hướng GPIO
```c
esp_err_t gpio_set_direction(gpio_num_t gpio_num, gpio_mode_t mode);
```
**Ví dụ**:
```c
gpio_set_direction(GPIO_NUM_25, GPIO_MODE_OUTPUT);
gpio_set_direction(GPIO_NUM_26, GPIO_MODE_INPUT);
```

#### 2. `gpio_set_level()` - Đặt mức logic
```c
esp_err_t gpio_set_level(gpio_num_t gpio_num, uint32_t level);
```
**Ví dụ**:
```c
gpio_set_level(GPIO_NUM_25, 1);  // HIGH
gpio_set_level(GPIO_NUM_25, 0);  // LOW
```

#### 3. `gpio_get_level()` - Đọc mức logic
```c
int gpio_get_level(gpio_num_t gpio_num);
```
**Ví dụ**:
```c
int level = gpio_get_level(GPIO_NUM_26);
if (level == 1) {
    printf("HIGH\n");
}
```

#### 4. `gpio_config()` - Cấu hình GPIO đầy đủ
```c
esp_err_t gpio_config(const gpio_config_t *pGPIOConfig);
```
**Ví dụ**:
```c
gpio_config_t io_conf = {
    .pin_bit_mask = (1ULL << GPIO_NUM_25),
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE
};
gpio_config(&io_conf);
```

---

## 🔌 I2C Functions

### `#include "driver/i2c.h"`

#### 1. `i2c_param_config()` - Cấu hình I2C
```c
esp_err_t i2c_param_config(i2c_port_t i2c_num, const i2c_config_t *i2c_conf);
```

#### 2. `i2c_driver_install()` - Cài đặt driver
```c
esp_err_t i2c_driver_install(i2c_port_t i2c_num, i2c_mode_t mode, size_t slv_rx_buf_len, size_t slv_tx_buf_len, int intr_alloc_flags);
```

#### 3. `i2c_master_write_to_device()` - Ghi dữ liệu
```c
esp_err_t i2c_master_write_to_device(i2c_port_t i2c_num, uint8_t device_address, const uint8_t *write_buffer, size_t write_size, TickType_t ticks_to_wait);
```

#### 4. `i2c_master_read_from_device()` - Đọc dữ liệu
```c
esp_err_t i2c_master_read_from_device(i2c_port_t i2c_num, uint8_t device_address, uint8_t *read_buffer, size_t read_size, TickType_t ticks_to_wait);
```

**Ví dụ đầy đủ**:
```c
// Cấu hình
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
uint8_t data[] = {0x01, 0x02, 0x03};
i2c_master_write_to_device(I2C_NUM_0, 0x68, data, 3, 1000 / portTICK_PERIOD_MS);

// Đọc dữ liệu
uint8_t buffer[10];
i2c_master_read_from_device(I2C_NUM_0, 0x68, buffer, 10, 1000 / portTICK_PERIOD_MS);
```

---

## 📡 UART Functions

### `#include "driver/uart.h"`

#### 1. `uart_param_config()` - Cấu hình UART
```c
esp_err_t uart_param_config(uart_port_t uart_num, const uart_config_t *uart_config);
```

#### 2. `uart_set_pin()` - Đặt chân UART
```c
esp_err_t uart_set_pin(uart_port_t uart_num, int tx_io_num, int rx_io_num, int rts_io_num, int cts_io_num);
```

#### 3. `uart_driver_install()` - Cài đặt driver
```c
esp_err_t uart_driver_install(uart_port_t uart_num, int rx_buffer_size, int tx_buffer_size, int queue_size, QueueHandle_t *uart_queue, int intr_alloc_flags);
```

#### 4. `uart_write_bytes()` - Ghi dữ liệu
```c
int uart_write_bytes(uart_port_t uart_num, const char *src, size_t size);
```

#### 5. `uart_read_bytes()` - Đọc dữ liệu
```c
int uart_read_bytes(uart_port_t uart_num, uint8_t *buf, uint32_t length, TickType_t ticks_to_wait);
```

**Ví dụ đầy đủ**:
```c
// Cấu hình UART
uart_config_t uart_config = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
};
uart_param_config(UART_NUM_1, &uart_config);
uart_set_pin(UART_NUM_1, GPIO_NUM_17, GPIO_NUM_16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
uart_driver_install(UART_NUM_1, 1024, 0, 0, NULL, 0);

// Ghi dữ liệu
char *data = "Hello UART\n";
uart_write_bytes(UART_NUM_1, data, strlen(data));

// Đọc dữ liệu
uint8_t buffer[128];
int len = uart_read_bytes(UART_NUM_1, buffer, sizeof(buffer), 100 / portTICK_PERIOD_MS);
if (len > 0) {
    buffer[len] = '\0';
    printf("Received: %s\n", buffer);
}
```

---

## 🎯 Tổng kết các hàm hay dùng nhất

### String
- `strlen()` - Độ dài chuỗi
- `strcpy()` / `strncpy()` - Copy chuỗi
- `strcmp()` - So sánh chuỗi
- `sprintf()` / `snprintf()` - Format chuỗi

### Memory
- `memset()` - Gán giá trị
- `memcpy()` - Copy bộ nhớ
- `malloc()` / `free()` - Cấp phát động

### Math
- `abs()` / `fabs()` - Giá trị tuyệt đối
- `pow()` - Lũy thừa
- `sqrt()` - Căn bậc 2

### ESP-IDF
- `ESP_LOGI()` / `ESP_LOGE()` - Logging
- `esp_restart()` - Reset ESP32
- `esp_get_free_heap_size()` - Kiểm tra RAM

### FreeRTOS
- `vTaskDelay()` - Delay
- `xTaskCreate()` - Tạo task

### GPIO
- `gpio_set_level()` - Bật/tắt
- `gpio_get_level()` - Đọc trạng thái

---

## 💡 Tips

1. **Luôn kiểm tra return value** của các hàm
2. **Dùng `snprintf()` thay vì `sprintf()`** để tránh buffer overflow
3. **Nhớ `free()` sau khi `malloc()`**
4. **Dùng `ESP_LOG` thay vì `printf()`** để có thể filter log level
5. **Kiểm tra NULL pointer** trước khi dereference

---

## 🔗 Tài liệu tham khảo

- [C Standard Library Reference](https://en.cppreference.com/w/c)
- [ESP-IDF API Reference](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/index.html)
- [FreeRTOS API Reference](https://www.freertos.org/a00106.html)
