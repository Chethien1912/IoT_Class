/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"

#define RELAY_1_PIN CONFIG_RELAY_1_PIN

void app_main(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << RELAY_1_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    gpio_set_level(RELAY_1_PIN, 1);
    while (1)
    {
        gpio_set_level(RELAY_1_PIN, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(RELAY_1_PIN, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}