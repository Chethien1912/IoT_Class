#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include <assert.h>
#include <string.h>

static const char *TAG = "example_connect";

// Event Group
static EventGroupHandle_t s_connect_event_group;
#define GOT_IPV4_BIT BIT0
#define GOT_IPV6_BIT BIT1
#define CONNECTED_BITS (GOT_IPV4_BIT)

static esp_netif_t *s_example_esp_netif = NULL;
static esp_event_handler_instance_t s_instance_any_id;
static esp_event_handler_instance_t s_instance_got_ip;

static void on_wifi_disconnect(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
  ESP_LOGI(TAG, "Wi-Fi disconnected, trying to reconnect...");
  esp_err_t err = esp_wifi_connect();
  if (err == ESP_ERR_WIFI_NOT_INIT) {
    return;
  }
  ESP_ERROR_CHECK(err);
}

static void on_got_ip(void *arg, esp_event_base_t event_base, int32_t event_id,
                      void *event_data) {
  ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
  ESP_LOGI(TAG, "Got IPv4 event: Interface \"%s\" address: " IPSTR,
           esp_netif_get_desc(event->esp_netif), IP2STR(&event->ip_info.ip));
  xEventGroupSetBits(s_connect_event_group, GOT_IPV4_BIT);
}

esp_err_t example_connect(void) {
  if (s_connect_event_group != NULL) {
    return ESP_ERR_INVALID_STATE;
  }
  s_connect_event_group = xEventGroupCreate();

  // Init NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // Init Netif
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  // Create default Wifi station
  esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
  assert(sta_netif);

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect, NULL,
      &s_instance_any_id));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      IP_EVENT, IP_EVENT_STA_GOT_IP, &on_got_ip, NULL, &s_instance_got_ip));

  s_example_esp_netif = sta_netif;

  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

  wifi_config_t wifi_config = {
      .sta =
          {
              .ssid = CONFIG_EXAMPLE_WIFI_SSID,
              .password = CONFIG_EXAMPLE_WIFI_PASSWORD,
              .threshold.authmode = WIFI_AUTH_WPA2_PSK,
          },
  };

  ESP_LOGI(TAG, "Connecting to %s...", wifi_config.sta.ssid);
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());
  ESP_ERROR_CHECK(esp_wifi_connect());

  // Wait for IP
  ESP_LOGI(TAG, "Waiting for IP(s)");
  xEventGroupWaitBits(s_connect_event_group, CONNECTED_BITS, true, true,
                      portMAX_DELAY);

  ESP_LOGI(TAG, "Connected to %s!", wifi_config.sta.ssid);
  return ESP_OK;
}

esp_err_t example_disconnect(void) {
  if (s_connect_event_group == NULL) {
    return ESP_OK;
  }
  vEventGroupDelete(s_connect_event_group);
  s_connect_event_group = NULL;
  esp_wifi_stop();
  esp_wifi_deinit();
  return ESP_OK;
}
