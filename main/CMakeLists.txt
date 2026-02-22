idf_component_register(
    SRCS "main.c"
    PRIV_REQUIRES spi_flash
    REQUIRES Sensor Relay HTTPServer protocol_examples_common 
    REQUIRES nvs_flash esp_wifi mqtt_manager
)