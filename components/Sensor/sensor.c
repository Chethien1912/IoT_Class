#include "sensor.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "DS3231";
static i2c_port_t ds3231_i2c_port = I2C_NUM_0;

/**
 * @brief Convert BCD to Decimal
 */
uint8_t bcd_to_dec(uint8_t bcd) { return ((bcd / 16) * 10) + (bcd % 16); }

/**
 * @brief Convert Decimal to BCD
 */
uint8_t dec_to_bcd(uint8_t dec) { return ((dec / 10) * 16) + (dec % 10); }

/**
 * @brief Write data to DS3231 register
 */
static esp_err_t ds3231_write_reg(uint8_t reg_addr, uint8_t data) {
  uint8_t write_buf[2] = {reg_addr, data};

  esp_err_t ret =
      i2c_master_write_to_device(ds3231_i2c_port, DS3231_I2C_ADDR, write_buf,
                                 sizeof(write_buf), 1000 / portTICK_PERIOD_MS);

  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to write to register 0x%02X", reg_addr);
  }

  return ret;
}

/**
 * @brief Read data from DS3231 register
 */
static esp_err_t ds3231_read_reg(uint8_t reg_addr, uint8_t *data, size_t len) {
  esp_err_t ret =
      i2c_master_write_read_device(ds3231_i2c_port, DS3231_I2C_ADDR, &reg_addr,
                                   1, data, len, 1000 / portTICK_PERIOD_MS);

  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to read from register 0x%02X", reg_addr);
  }

  return ret;
}

/**
 * @brief Initialize DS3231 RTC module
 */
esp_err_t ds3231_init(const ds3231_config_t *config) {
  if (config == NULL) {
    ESP_LOGE(TAG, "Config is NULL");
    return ESP_ERR_INVALID_ARG;
  }

  // Store I2C port
  ds3231_i2c_port = config->i2c_port;

  // Configure I2C
  i2c_config_t i2c_conf = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = config->sda_pin,
      .scl_io_num = config->scl_pin,
      .sda_pullup_en = GPIO_PULLUP_ENABLE,
      .scl_pullup_en = GPIO_PULLUP_ENABLE,
      .master.clk_speed = config->clk_speed,
  };

  esp_err_t ret = i2c_param_config(config->i2c_port, &i2c_conf);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "I2C param config failed");
    return ret;
  }

  ret = i2c_driver_install(config->i2c_port, I2C_MODE_MASTER, 0, 0, 0);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "I2C driver install failed");
    return ret;
  }

  // Clear oscillator stop flag and enable oscillator
  uint8_t status;
  ret = ds3231_read_reg(DS3231_REG_STATUS, &status, 1);
  if (ret != ESP_OK) {
    return ret;
  }

  if (status & 0x80) {
    ESP_LOGW(TAG, "Oscillator was stopped, clearing flag");
    status &= ~0x80;
    ret = ds3231_write_reg(DS3231_REG_STATUS, status);
    if (ret != ESP_OK) {
      return ret;
    }
  }

  // Ensure oscillator is enabled on battery (Clear EOSC bit in Control
  // Register)
  uint8_t control;
  ret = ds3231_read_reg(DS3231_REG_CONTROL, &control, 1);
  if (ret != ESP_OK) {
    return ret;
  }

  // Clear bit 7 (EOSC) to enable oscillator on battery
  if (control & 0x80) {
    ESP_LOGI(TAG, "Enabling oscillator on battery");
    control &= ~0x80;
    ret = ds3231_write_reg(DS3231_REG_CONTROL, control);
    if (ret != ESP_OK) {
      return ret;
    }
  }

  ESP_LOGI(TAG, "DS3231 initialized successfully");
  return ESP_OK;
}

/**
 * @brief Set time on DS3231
 */
esp_err_t ds3231_set_time(const ds3231_time_t *time) {
  if (time == NULL) {
    ESP_LOGE(TAG, "Time is NULL");
    return ESP_ERR_INVALID_ARG;
  }

  // Validate input
  if (time->seconds > 59 || time->minutes > 59 || time->hours > 23 ||
      time->day < 1 || time->day > 7 || time->date < 1 || time->date > 31 ||
      time->month < 1 || time->month > 12 || time->year < 2000 ||
      time->year > 2099) {
    ESP_LOGE(TAG, "Invalid time values");
    return ESP_ERR_INVALID_ARG;
  }

  esp_err_t ret;

  // Set seconds
  ret = ds3231_write_reg(DS3231_REG_SECONDS, dec_to_bcd(time->seconds));
  if (ret != ESP_OK)
    return ret;

  // Set minutes
  ret = ds3231_write_reg(DS3231_REG_MINUTES, dec_to_bcd(time->minutes));
  if (ret != ESP_OK)
    return ret;

  // Set hours (24-hour format)
  ret = ds3231_write_reg(DS3231_REG_HOURS, dec_to_bcd(time->hours));
  if (ret != ESP_OK)
    return ret;

  // Set day of week
  ret = ds3231_write_reg(DS3231_REG_DAY, dec_to_bcd(time->day));
  if (ret != ESP_OK)
    return ret;

  // Set date
  ret = ds3231_write_reg(DS3231_REG_DATE, dec_to_bcd(time->date));
  if (ret != ESP_OK)
    return ret;

  // Set month
  ret = ds3231_write_reg(DS3231_REG_MONTH, dec_to_bcd(time->month));
  if (ret != ESP_OK)
    return ret;

  // Set year (only last 2 digits)
  ret = ds3231_write_reg(DS3231_REG_YEAR, dec_to_bcd(time->year - 2000));
  if (ret != ESP_OK)
    return ret;

  ESP_LOGI(TAG, "Time set: %04d-%02d-%02d %02d:%02d:%02d", time->year,
           time->month, time->date, time->hours, time->minutes, time->seconds);

  return ESP_OK;
}

/**
 * @brief Get time from DS3231
 */
esp_err_t ds3231_get_time(ds3231_time_t *time) {
  if (time == NULL) {
    ESP_LOGE(TAG, "Time is NULL");
    return ESP_ERR_INVALID_ARG;
  }

  uint8_t data[7];
  esp_err_t ret = ds3231_read_reg(DS3231_REG_SECONDS, data, 7);
  if (ret != ESP_OK) {
    return ret;
  }

  // Convert BCD to decimal
  time->seconds = bcd_to_dec(data[0] & 0x7F);
  time->minutes = bcd_to_dec(data[1] & 0x7F);
  time->hours = bcd_to_dec(data[2] & 0x3F); // 24-hour format
  time->day = bcd_to_dec(data[3] & 0x07);
  time->date = bcd_to_dec(data[4] & 0x3F);
  time->month = bcd_to_dec(data[5] & 0x1F);
  time->year = bcd_to_dec(data[6]) + 2000;

  ESP_LOGD(TAG, "Time read: %04d-%02d-%02d %02d:%02d:%02d", time->year,
           time->month, time->date, time->hours, time->minutes, time->seconds);

  return ESP_OK;
}

/**
 * @brief Get temperature from DS3231
 */
esp_err_t ds3231_get_temperature(float *temperature) {
  if (temperature == NULL) {
    ESP_LOGE(TAG, "Temperature is NULL");
    return ESP_ERR_INVALID_ARG;
  }

  uint8_t data[2];
  esp_err_t ret = ds3231_read_reg(DS3231_REG_TEMP_MSB, data, 2);
  if (ret != ESP_OK) {
    return ret;
  }

  // Convert to temperature
  int16_t temp_raw = (data[0] << 8) | data[1];
  *temperature = temp_raw / 256.0f;

  ESP_LOGD(TAG, "Temperature: %.2f°C", *temperature);

  return ESP_OK;
}

/**
 * @brief Deinitialize DS3231 and I2C driver
 */
esp_err_t ds3231_deinit(void) {
  esp_err_t ret = i2c_driver_delete(ds3231_i2c_port);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to delete I2C driver");
    return ret;
  }

  ESP_LOGI(TAG, "DS3231 deinitialized");
  return ESP_OK;
}
