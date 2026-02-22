/**
 * @file ds3231_example.c
 * @brief Ví dụ sử dụng DS3231 RTC Module
 *
 * Chương trình này demo cách sử dụng DS3231:
 * - Khởi tạo module
 * - Đặt thời gian ban đầu
 * - Đọc thời gian liên tục
 * - Đọc nhiệt độ
 */

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sensor.h"
#include <stdio.h>

static const char *TAG = "DS3231_EXAMPLE";

/**
 * @brief Task đọc thời gian và nhiệt độ
 */
void rtc_task(void *pvParameters) {
  ds3231_time_t time;
  float temperature;

  while (1) {
    // Đọc thời gian
    if (ds3231_get_time(&time) == ESP_OK) {
      ESP_LOGI(TAG, "📅 Thời gian: %04d-%02d-%02d (Thứ %d) %02d:%02d:%02d",
               time.year, time.month, time.date, time.day, time.hours,
               time.minutes, time.seconds);
    } else {
      ESP_LOGE(TAG, "Lỗi đọc thời gian!");
    }

    // Đọc nhiệt độ (mỗi 5 giây)
    static int count = 0;
    if (count % 5 == 0) {
      if (ds3231_get_temperature(&temperature) == ESP_OK) {
        ESP_LOGI(TAG, "🌡️  Nhiệt độ: %.2f°C", temperature);
      } else {
        ESP_LOGE(TAG, "Lỗi đọc nhiệt độ!");
      }
    }
    count++;

    vTaskDelay(pdMS_TO_TICKS(1000)); // Delay 1 giây
  }
}

void app_main(void) {
  ESP_LOGI(TAG, "=== DS3231 RTC Example ===");

  // Cấu hình DS3231
  ds3231_config_t config = {
      .i2c_port = I2C_NUM_0,
      .sda_pin = GPIO_NUM_21, // SDA -> GPIO21
      .scl_pin = GPIO_NUM_22, // SCL -> GPIO22
      .clk_speed = 100000     // 100kHz
  };

  // Khởi tạo DS3231
  esp_err_t ret = ds3231_init(&config);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "❌ Lỗi khởi tạo DS3231!");
    return;
  }
  ESP_LOGI(TAG, "✅ DS3231 khởi tạo thành công!");

  // Đặt thời gian ban đầu (chỉ cần làm 1 lần)
  // Bỏ comment dòng dưới nếu muốn đặt lại thời gian
  /*
  ds3231_time_t init_time = {
      .year = 2026,
      .month = 2,      // Tháng 2
      .date = 11,      // Ngày 11
      .day = 3,        // Thứ 3 (1=CN, 2=T2, ..., 7=T7)
      .hours = 9,      // 9 giờ
      .minutes = 52,   // 52 phút
      .seconds = 0     // 0 giây
  };

  ret = ds3231_set_time(&init_time);
  if (ret == ESP_OK) {
      ESP_LOGI(TAG, "✅ Đã đặt thời gian thành công!");
  } else {
      ESP_LOGE(TAG, "❌ Lỗi đặt thời gian!");
  }
  */

  // Tạo task đọc thời gian
  xTaskCreate(rtc_task, "rtc_task", 4096, NULL, 5, NULL);

  ESP_LOGI(TAG, "Bắt đầu đọc thời gian...");
}
