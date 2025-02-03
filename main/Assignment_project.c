#include <stdio.h>
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "wifi_provisioning/manager.h"
#include "wifi_provisioning/scheme_ble.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static bool wifi_connected = false;

esp_err_t client_event_get_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
        case HTTP_EVENT_ON_DATA:
            printf("\n==== Cat Fact ====\n");
            printf("%.*s\n", evt->data_len, (char*)evt->data);
            printf("=================\n");
            break;
        default:
            break;
    }
    return ESP_OK;
}

static void rest_get()
{
    printf("[WiFi] Fetching cat fact...\n");

    esp_http_client_config_t config_get = {
        .url = "http://meowfacts.herokuapp.com/",
        .method = HTTP_METHOD_GET,
        .cert_pem = NULL,
        // Direct assignment without casting
        .event_handler = client_event_get_handler
    };

    esp_http_client_handle_t client = esp_http_client_init(&config_get);
    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}

// Wi-Fi event handler
void event_handler(void* arg, esp_event_base_t event_base,
                   int32_t event_id, void* event_data) 
{
    if (event_base == WIFI_EVENT) {
        if (event_id == WIFI_EVENT_STA_START) {
            esp_wifi_connect();
        } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
            printf("[WiFi] Disconnected. Attempting to reconnect...\n");
            wifi_connected = false;
            esp_wifi_connect();
        }
    } 
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        printf("[WiFi] Connected! IP Assigned.\n");
        wifi_connected = true;
    }
}

void start_wifi_provisioning() 
{
    // Configure BLE provisioning
    wifi_prov_mgr_config_t config = {
        .scheme = wifi_prov_scheme_ble,
        .scheme_event_handler = WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BTDM
    };

    // Initialize provisioning manager
    wifi_prov_mgr_init(config);

    bool provisioned = false;
    wifi_prov_mgr_is_provisioned(&provisioned);

    if (provisioned) {
        printf("[WiFi] Device already provisioned. Connecting to WiFi...\n");
        wifi_prov_mgr_deinit();
        return;
    }

    // Start provisioning if not already done
    printf("[WiFi] Starting WiFi Provisioning...\n");
    wifi_prov_mgr_start_provisioning(
        WIFI_PROV_SECURITY_1, 
        "12345678",      // Provisioning password 
        "PROV_ESP32",    // Provisioning name
        NULL
    );
}

void wifi_task(void* pvParameter) 
{
    while (!wifi_connected) {
        printf("[WiFi] Waiting for WiFi connection...\n");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    vTaskDelay(3000 / portTICK_PERIOD_MS);

    rest_get();

    vTaskDelete(NULL);
}

void app_main(void) 
{
    // Initialize NVS (Non-Volatile Storage)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        printf("[WiFi] Erasing NVS and reinitializing...\n");
        nvs_flash_erase();
        nvs_flash_init();
    }

    // Network and WiFi initialization
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    // WiFi configuration
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL);

    esp_wifi_start();

    start_wifi_provisioning();

    xTaskCreate(&wifi_task, "wifi_task", 4096, NULL, 5, NULL);
}