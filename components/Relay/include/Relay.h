#pragma once

#include "driver/gpio.h"
#include <stdbool.h>

#define MAX_RELAYS 3 // Hỗ trợ tối đa 3 relay

/**
 * @brief Khởi tạo relay
 * @param relay_idx Chỉ số relay (0-2)
 * @param relay_gpio GPIO điều khiển relay
 * @param active_high true: mức 1 bật relay, false: mức 0 bật relay
 */
void relay_init(uint8_t relay_idx, gpio_num_t relay_gpio, bool active_high);

/**
 * @brief Bật relay
 * @param relay_idx Chỉ số relay (0-2)
 */
void relay_on(uint8_t relay_idx);

/**
 * @brief Tắt relay
 * @param relay_idx Chỉ số relay (0-2)
 */
void relay_off(uint8_t relay_idx);

/**
 * @brief Đảo trạng thái relay
 * @param relay_idx Chỉ số relay (0-2)
 */
void relay_toggle(uint8_t relay_idx);

/**
 * @brief Lấy trạng thái relay
 * @param relay_idx Chỉ số relay (0-2)
 * @return true: ON, false: OFF
 */
bool relay_get_state(uint8_t relay_idx);
