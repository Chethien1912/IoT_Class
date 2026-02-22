/**
 * @file main.c
 * @brief App main - Kết nối WiFi rồi khởi động MQTT
 */

#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "mqtt_manager.h"
#include "nvs_flash.h"
#include "sensor.h"
#include <string.h>

static const char *TAG = "MAIN";

// ===== CẤU HÌNH WIFI - SỬA THÔNG TIN CỦA BẠN =====
#define WIFI_SSID "DIEU LINH"
#define WIFI_PASSWORD "giahuy2020"

static EventGroupHandle_t wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0

// ===== WIFI EVENT HANDLER =====
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
  {
    esp_wifi_connect();
  }
  else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
  {
    ESP_LOGW(TAG, "⚠️ WiFi mất kết nối, đang kết nối lại...");
    esp_wifi_connect();
  }
  else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
  {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(TAG, "✅ WiFi kết nối! IP: " IPSTR, IP2STR(&event->ip_info.ip));
    xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
  }
}

// ===== KHỞI TẠO WIFI =====
static void wifi_init(void)
{
  wifi_event_group = xEventGroupCreate();

  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  esp_event_handler_instance_t instance_any_id;
  esp_event_handler_instance_t instance_got_ip;
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &instance_any_id));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, &instance_got_ip));

  wifi_config_t wifi_config = {
      .sta = {
          .threshold.authmode = WIFI_AUTH_WPA2_PSK,
      },
  };
  strncpy((char *)wifi_config.sta.ssid, WIFI_SSID, sizeof(wifi_config.sta.ssid));
  strncpy((char *)wifi_config.sta.password, WIFI_PASSWORD, sizeof(wifi_config.sta.password));

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_LOGI(TAG, "⏳ Đang kết nối WiFi: %s ...", WIFI_SSID);

  // Chờ kết nối WiFi thành công (timeout 30 giây)
  EventBits_t bits = xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT,
                                         pdFALSE, pdFALSE,
                                         pdMS_TO_TICKS(30000));
  if (!(bits & WIFI_CONNECTED_BIT))
  {
    ESP_LOGE(TAG, "❌ Kết nối WiFi thất bại sau 30 giây!");
    // Vẫn tiếp tục, MQTT sẽ retry tự động
  }
}

// ===== APP MAIN =====
void app_main(void)
{
  ESP_LOGI(TAG, "=============================");
  ESP_LOGI(TAG, "  ESP32 IoT MQTT App");
  ESP_LOGI(TAG, "=============================");

  // Khởi tạo NVS (bắt buộc cho WiFi)
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // Bước 1: Kết nối WiFi
  wifi_init();

  // Bước 2: Khởi tạo DS3231
  ds3231_config_t ds_config = {
      .i2c_port = I2C_NUM_0,
      .sda_pin = GPIO_NUM_21,
      .scl_pin = GPIO_NUM_22,
      .clk_speed = 100000};
  ret = ds3231_init(&ds_config);
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "❌ Lỗi khởi tạo DS3231!");
  }
  else
  {
    ESP_LOGI(TAG, "✅ DS3231 sẵn sàng!");
  }
  // Set thời gian - chỉ cần làm 1 lần, sau đó comment lại!
  /*
  ds3231_time_t init_time = {
      .year = 2026,
      .month = 2,
      .date = 22,
      .day = 1,      // 1=CN, 2=T2, 3=T3, 4=T4, 5=T5, 6=T6, 7=T7
      .hours = 8,   // ← Sửa giờ thật của bạn
      .minutes = 5, // ← Sửa phút thật của bạn
      .seconds = 30};
  ds3231_set_time(&init_time);
  ESP_LOGI(TAG, "✅ Đã set thời gian!");
  */

  // Bước 3: Khởi động MQTT
  ret = mqtt_manager_init();
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "❌ Lỗi khởi tạo MQTT!");
  }
  else
  {
    ESP_LOGI(TAG, "✅ MQTT sẵn sàng!");
  }

  ESP_LOGI(TAG, "🎉 Hệ thống hoạt động!");
}