#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_server.h"


#define WIFI_SSID     "A56 de Janine"  
#define WIFI_PASS     "Zeus1234"       
#define MAX_CLIENTS   4                

static const char *TAG = "WebServer_AP";


esp_err_t root_get_handler(httpd_req_t *req)
{
    const char resp[] =
        "<html>"
        "<head><title>ESP32 WebServer</title></head>"
        "<body>"
        "<h1>¡Hola! Estás conectado al ESP32 directamente.</h1>"
        "<p>Modo Access Point activo.</p>"
        "</body></html>";

    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

httpd_handle_t start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    ESP_LOGI(TAG, "Iniciando servidor HTTP...");

    if (httpd_start(&server, &config) == ESP_OK) {

        httpd_uri_t root = {
            .uri       = "/",
            .method    = HTTP_GET,
            .handler   = root_get_handler,
            .user_ctx  = NULL
        };

        httpd_register_uri_handler(server, &root);
        ESP_LOGI(TAG, "Servidor web iniciado correctamente.");

    } else {
        ESP_LOGE(TAG, "Error al iniciar el servidor web.");
    }

    return server;
}

// -----------------------------------------------------------------------------
// CONFIGURACIÓN DEL MODO ACCESS POINT (Wi-Fi)
// -----------------------------------------------------------------------------
void wifi_init_softap(void)
{
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = WIFI_SSID,
            .ssid_len = strlen(WIFI_SSID),
            .channel = 1,
            .password = WIFI_PASS,
            .max_connection = MAX_CLIENTS,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };

    if (strlen(WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "--------------------------------------------");
    ESP_LOGI(TAG, "Punto de acceso iniciado correctamente");
    ESP_LOGI(TAG, "SSID: %s", WIFI_SSID);
    ESP_LOGI(TAG, "Contraseña: %s", WIFI_PASS);
    ESP_LOGI(TAG, "IP por defecto: 192.168.4.1");
    ESP_LOGI(TAG, "--------------------------------------------");
}

// -----------------------------------------------------------------------------
// FUNCIÓN PRINCIPAL
// -----------------------------------------------------------------------------
void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_softap();
    start_webserver();

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
