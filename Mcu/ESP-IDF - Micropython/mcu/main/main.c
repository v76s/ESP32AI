#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_netif.h"

#define WIFI_SSID "SARP"
#define WIFI_PASS "Sarposky2011"
#define SERVER_IP "192.168.1.193"
#define SERVER_PORT 5000

static const char *TAG = "TCP_CLIENT";

void wifi_init_sta(void)
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();

    ESP_LOGI(TAG, "WiFi connecting to SSID:%s", WIFI_SSID);

    esp_wifi_connect();
}

void tcp_client_task(void *pvParameters)
{
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(SERVER_PORT);

    while (1) {
        int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            vTaskDelay(pdMS_TO_TICKS(5000));
            continue;
        }

        ESP_LOGI(TAG, "Socket created, connecting to %s:%d", SERVER_IP, SERVER_PORT);

        int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0) {
            ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
            close(sock);
            vTaskDelay(pdMS_TO_TICKS(5000));
            continue;
        }

        ESP_LOGI(TAG, "Successfully connected");

        srand((unsigned int)time(NULL));
        TickType_t start_tick = xTaskGetTickCount();
        const TickType_t ten_minutes = pdMS_TO_TICKS(10 * 60 * 1000); // 10 minutes

        while (1) {
            // Check if 10 minutes have passed
            if ((xTaskGetTickCount() - start_tick) > ten_minutes) {
                ESP_LOGI(TAG, "10 minutes elapsed, shutting down socket.");
                break;
            }

            int x = rand() % 1001;
            int y = rand() % 1001;
            char msg[32];
            snprintf(msg, sizeof(msg), "%d,%d", x, y);

            int sent = send(sock, msg, strlen(msg), 0);
            if (sent < 0) {
                ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                break;
            }

            ESP_LOGI(TAG, "Sent: %s", msg);
            vTaskDelay(pdMS_TO_TICKS(500));
        }

        if (sock != -1) {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init_sta();
    xTaskCreate(tcp_client_task, "tcp_client", 4096, NULL, 5, NULL);
}
