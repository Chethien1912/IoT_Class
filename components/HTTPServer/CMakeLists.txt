idf_component_register(
    SRCS "HTTPServer.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_http_server Relay freertos
)
