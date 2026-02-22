#pragma once

#include "esp_err.h"

/**
 * @brief Start the web server
 *
 * @return esp_err_t ESP_OK on success, or error code
 */
esp_err_t start_webserver(void);

/**
 * @brief Stop the web server
 *
 * @return esp_err_t ESP_OK on success, or error code
 */
esp_err_t stop_webserver(void);
