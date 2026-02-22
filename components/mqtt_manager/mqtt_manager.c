/**
 * @file mqtt_manager.c
 * @brief Quản lý kết nối MQTT với HiveMQ Cloud
 *
 * Tích hợp với:
 * - DS3231: Đọc nhiệt độ + thời gian thực, gửi lên app mỗi 1 giây
 * - Relay 1, 2, 3 (GPIO 25, 32, 33): Nhận lệnh bật/tắt từ app
 *
 * Topics:
 * - home/esp32-01/sensor  → ESP32 gửi nhiệt độ + thời gian lên app
 * - home/esp32-01/relay   → App gửi lệnh xuống ESP32
 * - home/esp32-01/status  → ESP32 phản hồi trạng thái relay
 */

#include "mqtt_manager.h"
#include "Relay.h"
#include "sensor.h"
#include "esp_crt_bundle.h"
#include "esp_log.h"
#include "esp_random.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "mqtt_client.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "MQTT_MANAGER";

// ===== CẤU HÌNH HIVEMQ =====
#define MQTT_BROKER_HOST  "37f7969a8e364ee88df3551c3a0a2ea2.s1.eu.hivemq.cloud"
#define MQTT_PORT         8883
#define MQTT_USERNAME     "IoTClass"
#define MQTT_PASSWORD     "Chethien@1912"

// ===== TOPICS =====
#define TOPIC_SENSOR  "home/esp32-01/sensor"
#define TOPIC_RELAY   "home/esp32-01/relay"
#define TOPIC_STATUS  "home/esp32-01/status"

// ===== CẤU HÌNH RELAY =====
#define RELAY_1_GPIO      GPIO_NUM_25
#define RELAY_2_GPIO      GPIO_NUM_32
#define RELAY_3_GPIO      GPIO_NUM_33
#define RELAY_ACTIVE_HIGH true

static esp_mqtt_client_handle_t mqtt_client = NULL;
static bool mqtt_connected = false;

// ===== PARSE LỆNH RELAY TỪ APP =====
static void parse_relay_command(const char *data, int *relay_idx, bool *state) {
    *relay_idx = -1;
    *state = false;

    char *p = strstr(data, "relay");
    if (p) {
        p = strstr(p, ":");
        if (p) {
            p++;
            while (*p == ' ') p++;
            *relay_idx = atoi(p);
        }
    }

    p = strstr(data, "state");
    if (p) {
        p = strstr(p, ":");
        if (p) {
            p++;
            while (*p == ' ') p++;
            *state = (strncmp(p, "true", 4) == 0);
        }
    }
}

static void handle_relay_command(const char *data, int data_len) {
    char buf[128] = {0};
    int len = data_len < 127 ? data_len : 127;
    memcpy(buf, data, len);

    int relay_idx = -1;
    bool state = false;
    parse_relay_command(buf, &relay_idx, &state);

    if (relay_idx < 0 || relay_idx > 2) {
        ESP_LOGW(TAG, "Relay index khong hop le: %d", relay_idx);
        return;
    }

    if (state) {
        relay_on(relay_idx);
        ESP_LOGI(TAG, "Relay %d: BAT", relay_idx + 1);
    } else {
        relay_off(relay_idx);
        ESP_LOGI(TAG, "Relay %d: TAT", relay_idx + 1);
    }

    mqtt_publish_relay_status(relay_idx, state);
}

// ===== MQTT EVENT HANDLER =====
static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                               int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

    switch (event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "Ket noi HiveMQ thanh cong!");
        mqtt_connected = true;
        esp_mqtt_client_subscribe(mqtt_client, TOPIC_RELAY, 1);
        esp_mqtt_client_publish(mqtt_client, TOPIC_STATUS,
                                "{\"online\":true}", 0, 1, 0);
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGW(TAG, "Mat ket noi MQTT, dang reconnect...");
        mqtt_connected = false;
        break;

    case MQTT_EVENT_DATA:
        if (strncmp(event->topic, TOPIC_RELAY, event->topic_len) == 0) {
            handle_relay_command(event->data, event->data_len);
        }
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "MQTT loi!");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED) {
            ESP_LOGE(TAG, "Connection refused code: %d",
                     event->error_handle->connect_return_code);
        }
        break;

    default:
        break;
    }
}

// ===== TASK GỬI DỮ LIỆU MỖI 1 GIÂY =====
static void sensor_publish_task(void *pvParameters) {
    // Chờ kết nối MQTT
    while (!mqtt_connected) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    while (1) {
        if (mqtt_connected) {
            float temperature = 0;
            ds3231_time_t rtc_time;

            esp_err_t ret_temp = ds3231_get_temperature(&temperature);
            esp_err_t ret_time = ds3231_get_time(&rtc_time);

            if (ret_temp == ESP_OK && ret_time == ESP_OK) {
                char payload[200];
                snprintf(payload, sizeof(payload),
                    "{"
                    "\"temperature\":%.1f,"
                    "\"year\":%d,"
                    "\"month\":%d,"
                    "\"date\":%d,"
                    "\"day\":%d,"
                    "\"hours\":%d,"
                    "\"minutes\":%d,"
                    "\"seconds\":%d"
                    "}",
                    temperature,
                    rtc_time.year, rtc_time.month, rtc_time.date,
                    rtc_time.day,
                    rtc_time.hours, rtc_time.minutes, rtc_time.seconds
                );

                esp_mqtt_client_publish(mqtt_client, TOPIC_SENSOR,
                                        payload, 0, 1, 0);
                ESP_LOGI(TAG, "Gui: %s", payload);
            } else {
                if (ret_temp != ESP_OK)
                    ESP_LOGE(TAG, "Loi doc nhiet do DS3231");
                if (ret_time != ESP_OK)
                    ESP_LOGE(TAG, "Loi doc thoi gian DS3231");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// ===== KHỞI TẠO =====
esp_err_t mqtt_manager_init(void) {
    ESP_LOGI(TAG, "Khoi tao MQTT Manager...");

    relay_init(0, RELAY_1_GPIO, RELAY_ACTIVE_HIGH);
    relay_init(1, RELAY_2_GPIO, RELAY_ACTIVE_HIGH);
    relay_init(2, RELAY_3_GPIO, RELAY_ACTIVE_HIGH);
    ESP_LOGI(TAG, "Da khoi tao 3 relay (GPIO 25, 32, 33)");

    char client_id[32];
    snprintf(client_id, sizeof(client_id),
             "esp32-%08lx", (unsigned long)esp_random());

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker = {
            .address = {
                .hostname  = MQTT_BROKER_HOST,
                .port      = MQTT_PORT,
                .transport = MQTT_TRANSPORT_OVER_SSL,
            },
            .verification = {
                .crt_bundle_attach = esp_crt_bundle_attach,
            },
        },
        .credentials = {
            .username  = MQTT_USERNAME,
            .client_id = client_id,
            .authentication = {
                .password = MQTT_PASSWORD,
            },
        },
        .session = {
            .keepalive             = 60,
            .disable_clean_session = false,
        },
        .network = {
            .reconnect_timeout_ms = 5000,
        },
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    if (mqtt_client == NULL) {
        ESP_LOGE(TAG, "Khong the tao MQTT client!");
        return ESP_FAIL;
    }

    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID,
                                   mqtt_event_handler, NULL);

    esp_err_t ret = esp_mqtt_client_start(mqtt_client);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Khong the start MQTT client!");
        return ret;
    }

    xTaskCreate(sensor_publish_task, "sensor_mqtt", 4096, NULL, 5, NULL);

    ESP_LOGI(TAG, "MQTT Manager khoi tao xong!");
    return ESP_OK;
}

void mqtt_publish_sensor(float temperature) {
    if (!mqtt_connected) return;
    char payload[64];
    snprintf(payload, sizeof(payload), "{\"temperature\":%.1f}", temperature);
    esp_mqtt_client_publish(mqtt_client, TOPIC_SENSOR, payload, 0, 1, 0);
}

void mqtt_publish_relay_status(uint8_t relay_idx, bool state) {
    if (!mqtt_connected) return;
    char payload[64];
    snprintf(payload, sizeof(payload),
             "{\"relay\":%d,\"state\":%s}",
             relay_idx, state ? "true" : "false");
    esp_mqtt_client_publish(mqtt_client, TOPIC_STATUS, payload, 0, 1, 0);
    ESP_LOGI(TAG, "Trang thai: %s", payload);
}