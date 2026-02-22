idf_component_register(
    SRCS "connect.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_wifi nvs_flash esp_event driver esp_netif
)
