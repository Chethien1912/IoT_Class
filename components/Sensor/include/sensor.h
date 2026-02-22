#ifndef SENSOR_H
#define SENSOR_H

#include "driver/i2c.h"
#include "esp_err.h"
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

// DS3231 I2C Address
#define DS3231_I2C_ADDR 0x68

// DS3231 Register Addresses
#define DS3231_REG_SECONDS 0x00
#define DS3231_REG_MINUTES 0x01
#define DS3231_REG_HOURS 0x02
#define DS3231_REG_DAY 0x03
#define DS3231_REG_DATE 0x04
#define DS3231_REG_MONTH 0x05
#define DS3231_REG_YEAR 0x06
#define DS3231_REG_CONTROL 0x0E
#define DS3231_REG_STATUS 0x0F
#define DS3231_REG_TEMP_MSB 0x11
#define DS3231_REG_TEMP_LSB 0x12

/**
 * @brief DS3231 Time structure
 */
typedef struct {
  uint8_t seconds; // 0-59
  uint8_t minutes; // 0-59
  uint8_t hours;   // 0-23 (24-hour format)
  uint8_t day;     // 1-7 (day of week)
  uint8_t date;    // 1-31
  uint8_t month;   // 1-12
  uint16_t year;   // 2000-2099
} ds3231_time_t;

/**
 * @brief DS3231 configuration structure
 */
typedef struct {
  i2c_port_t i2c_port; // I2C port number
  gpio_num_t sda_pin;  // SDA pin
  gpio_num_t scl_pin;  // SCL pin
  uint32_t clk_speed;  // I2C clock speed (Hz)
} ds3231_config_t;

/**
 * @brief Initialize DS3231 RTC module
 *
 * @param config Pointer to DS3231 configuration structure
 * @return esp_err_t ESP_OK on success
 */
esp_err_t ds3231_init(const ds3231_config_t *config);

/**
 * @brief Set time on DS3231
 *
 * @param time Pointer to time structure
 * @return esp_err_t ESP_OK on success
 */
esp_err_t ds3231_set_time(const ds3231_time_t *time);

/**
 * @brief Get time from DS3231
 *
 * @param time Pointer to time structure to store the result
 * @return esp_err_t ESP_OK on success
 */
esp_err_t ds3231_get_time(ds3231_time_t *time);

/**
 * @brief Get temperature from DS3231
 *
 * @param temperature Pointer to store temperature value (in Celsius)
 * @return esp_err_t ESP_OK on success
 */
esp_err_t ds3231_get_temperature(float *temperature);

/**
 * @brief Convert BCD to Decimal
 *
 * @param bcd BCD value
 * @return uint8_t Decimal value
 */
uint8_t bcd_to_dec(uint8_t bcd);

/**
 * @brief Convert Decimal to BCD
 *
 * @param dec Decimal value
 * @return uint8_t BCD value
 */
uint8_t dec_to_bcd(uint8_t dec);

/**
 * @brief Deinitialize DS3231 and I2C driver
 *
 * @return esp_err_t ESP_OK on success
 */
esp_err_t ds3231_deinit(void);

#ifdef __cplusplus
}
#endif

#endif // SENSOR_H
