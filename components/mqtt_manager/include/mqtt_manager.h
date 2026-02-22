#pragma once

#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t mqtt_manager_init(void);
void mqtt_publish_sensor(float temperature);
void mqtt_publish_relay_status(uint8_t relay_idx, bool state);

#ifdef __cplusplus
}
#endif