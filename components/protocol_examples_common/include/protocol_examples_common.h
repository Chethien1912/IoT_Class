#pragma once

#include "esp_err.h"
#include "esp_netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @brief Kết nối tới Ethernet hoặc Wifi (dựa trên cấu hình menuconfig)
 */
esp_err_t example_connect(void);

/*
 * @brief Ngắt kết nối
 */
esp_err_t example_disconnect(void);

/*
 * @brief Cấu hình kết nối (được gọi bởi example_connect)
 */
esp_err_t example_configure_stdin_stdout(void);

#ifdef __cplusplus
}
#endif
