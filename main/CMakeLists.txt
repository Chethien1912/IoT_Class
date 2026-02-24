idf_component_register(
    SRCS "main.c"
    PRIV_REQUIRES spi_flash
    REQUIRES Sensor Relay HTTPServer protocol_examples_common
            nvs_flash esp_wifi esp_event esp_netif mqtt_manager
)