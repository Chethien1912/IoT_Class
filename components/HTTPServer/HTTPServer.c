#include "HTTPServer.h"
#include "Relay.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

static const char *TAG = "HTTPServer";
static httpd_handle_t server = NULL;

/* HTML Content - Minified/Embedded for simplicity */
static const char *html_page =
    "<!DOCTYPE html>\n"
    "<html lang=\"en\">\n"
    "<head>\n"
    "<meta charset=\"UTF-8\">\n"
    "<meta name=\"viewport\" content=\"width=device-width, "
    "initial-scale=1.0\">\n"
    "<title>ESP32 IoT Controller</title>\n"

    "<style>\n"
    "body { font-family: 'Segoe UI', sans-serif; background: "
    "linear-gradient(135deg, #1e3c72, #2a5298); margin: 0; color: white; "
    "text-align: center; }\n"
    "h1 { margin-top: 20px; }\n"
    "#clock { font-size: 2rem; margin: 15px 0; font-weight: bold; }\n"
    ".container { display: flex; justify-content: center; flex-wrap: wrap; "
    "gap: 20px; padding: 20px; }\n"
    ".card { background: white; color: #333; width: 250px; border-radius: "
    "15px; padding: 20px; box-shadow: 0 8px 20px rgba(0,0,0,0.3); transition: "
    "transform 0.2s ease; }\n"
    ".card:hover { transform: translateY(-5px); }\n"
    ".card1 { background: white; color: #333; width: 1000px; border-radius: "
    "1px; padding: 50px;"
    "box-shadow: 0 8px 20px rgba(0,0,0,0.3); transition: transform 0.2s ease; "
    "margin: 40px auto;}\n"
    ".status { font-size: 1.2rem; font-weight: bold; margin: 10px 0; }\n"
    "button { border: none; padding: 12px 25px; margin: 5px; font-size: 16px; "
    "border-radius: 8px; cursor: pointer; transition: 0.2s; }\n"
    ".on { background-color: #28a745; color: white; }\n"
    ".off { background-color: #dc3545; color: white; }\n"
    "button:hover { opacity: 0.8; }\n"
    "</style>\n"

    "</head>\n"
    "<body>\n"

    "<h1>ESP32 IoT Controller</h1>\n"
    "<div id=\"clock\">Loading time...</div>\n"
    "<img src=\"D:\\C_C++\\ESP\\IoTClass\\Data\\Logo.png\" width=\"500\">"
    "<div class=\"container\">\n"

    "<div class=\"card\">\n"
    "<h2>Relay 1</h2>\n"
    "<div class=\"status\" id=\"state0\">Unknown</div>\n"
    "<button class=\"on\" onclick=\"toggleRelay(0, true)\">ON</button>\n"
    "<button class=\"off\" onclick=\"toggleRelay(0, false)\">OFF</button>\n"
    "</div>\n"

    "<div class=\"card\">\n"
    "<h2>Relay 2</h2>\n"
    "<div class=\"status\" id=\"state1\">Unknown</div>\n"
    "<button class=\"on\" onclick=\"toggleRelay(1, true)\">ON</button>\n"
    "<button class=\"off\" onclick=\"toggleRelay(1, false)\">OFF</button>\n"
    "</div>\n"

    "<div class=\"card\">\n"
    "<h2>Relay 3</h2>\n"
    "<div class=\"status\" id=\"state2\">Unknown</div>\n"
    "<button class=\"on\" onclick=\"toggleRelay(2, true)\">ON</button>\n"
    "<button class=\"off\" onclick=\"toggleRelay(2, false)\">OFF</button>\n"
    "</div>\n"

    "</div>\n"
    "<script>\n"
    "function toggleRelay(id, state) {\n"
    "    fetch(`/api/relay?id=${id}&state=${state ? 1 : 0}`)\n"
    "        .then(() => setTimeout(updateStatus, 200));\n"
    "}\n"

    "function updateStatus() {\n"
    "    fetch('/api/status')\n"
    "        .then(response => response.json())\n"
    "        .then(data => {\n"
    "            document.getElementById('clock').innerText = data.time;\n"
    "            for (let i = 0; i < 3; i++) {\n"
    "                let stateElement = document.getElementById('state' + i);\n"
    "                if (data.relays[i]) {\n"
    "                    stateElement.innerText = 'ON';\n"
    "                    stateElement.style.color = 'green';\n"
    "                } else {\n"
    "                    stateElement.innerText = 'OFF';\n"
    "                    stateElement.style.color = 'red';\n"
    "                }\n"
    "            }\n"
    "        });\n"
    "}\n"

    "setInterval(updateStatus, 1000);\n"
    "window.onload = updateStatus;\n"
    "</script>\n"

    "</body>\n"
    "</html>\n";

/* Handler for the root URL */
static esp_err_t root_get_handler(httpd_req_t *req) {
  httpd_resp_send(req, html_page, HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
}

static const httpd_uri_t root = {.uri = "/",
                                 .method = HTTP_GET,
                                 .handler = root_get_handler,
                                 .user_ctx = NULL};

/* Handler for getting system status (Time and Relays) */
static esp_err_t status_get_handler(httpd_req_t *req) {
  char resp_str[256];
  time_t now;
  struct tm timeinfo;
  time(&now);
  struct tm *timeinfo_ptr = localtime(&now);
  if (timeinfo_ptr) {
    timeinfo = *timeinfo_ptr;
  }
  char time_str[64];
  strftime(time_str, sizeof(time_str), "%H:%M:%S", &timeinfo);

  bool r1 = relay_get_state(0);
  bool r2 = relay_get_state(1);
  bool r3 = relay_get_state(2);

  snprintf(resp_str, sizeof(resp_str),
           "{\"time\": \"%s\", \"relays\": [%s, %s, %s]}", time_str,
           r1 ? "true" : "false", r2 ? "true" : "false", r3 ? "true" : "false");

  httpd_resp_set_type(req, "application/json");
  httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
}

static const httpd_uri_t status_uri = {.uri = "/api/status",
                                       .method = HTTP_GET,
                                       .handler = status_get_handler,
                                       .user_ctx = NULL};

/* Handler for controlling Relays via API */
// Expected: /api/relay?id=0&state=1
static esp_err_t relay_control_handler(httpd_req_t *req) {
  char buf[100];
  size_t remaining = httpd_req_get_url_query_len(req);

  if (remaining >= sizeof(buf)) {
    return ESP_FAIL;
  }

  if (httpd_req_get_url_query_str(req, buf, sizeof(buf)) == ESP_OK) {
    char param[32];
    int id = -1;
    bool state = false;

    if (httpd_query_key_value(buf, "id", param, sizeof(param)) == ESP_OK) {
      id = atoi(param);
    }
    if (httpd_query_key_value(buf, "state", param, sizeof(param)) == ESP_OK) {
      state = (atoi(param) == 1);
    }

    if (id >= 0 && id <= 2) {
      if (state) {
        relay_on(id);
        ESP_LOGI(TAG, "Relay %d Turned ON", id);
      } else {
        relay_off(id);
        ESP_LOGI(TAG, "Relay %d Turned OFF", id);
      }
      httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);
      return ESP_OK;
    }
  }

  httpd_resp_send_500(req);
  return ESP_FAIL;
}

static const httpd_uri_t relay_control_uri = {.uri = "/api/relay",
                                              .method = HTTP_GET,
                                              .handler = relay_control_handler,
                                              .user_ctx = NULL};

esp_err_t start_webserver(void) {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.lru_purge_enable = true;

  // Start the httpd servers
  ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
  if (httpd_start(&server, &config) == ESP_OK) {
    // Set URI handlers
    ESP_LOGI(TAG, "Registering URI handlers");
    httpd_register_uri_handler(server, &root);
    httpd_register_uri_handler(server, &status_uri);
    httpd_register_uri_handler(server, &relay_control_uri);
    return ESP_OK;
  }

  ESP_LOGI(TAG, "Error starting server!");
  return ESP_FAIL;
}

esp_err_t stop_webserver(void) {
  // Stop the httpd server
  return httpd_stop(server);
}
