#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/twai.h"
#include "nvs_flash.h"

// Define the pins for CAN bus (TWAI) communication
#define TWAI_TX_PIN GPIO_NUM_21  // TX pin
#define TWAI_RX_PIN GPIO_NUM_22  // RX pin

// Define CAN message IDs
#define CAN_ID_REQUEST 0x123      // ID for request messages
#define CAN_ID_RESPONSE 0x456     // ID for response messages

static const char *TAG = "CAN_CLIENT";

// Initialize the TWAI (CAN) driver
void twai_init(void)
{
    // Configure TWAI driver
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TWAI_TX_PIN, TWAI_RX_PIN, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();  // Set to 500 Kbps
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    // Install TWAI driver
    ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
    
    // Start TWAI driver
    ESP_ERROR_CHECK(twai_start());
    
    ESP_LOGI(TAG, "TWAI driver initialized successfully");
}

// Task to send CAN bus requests
void can_request_task(void *pvParameters)
{
    // Prepare message to transmit
    twai_message_t tx_message;
    tx_message.identifier = CAN_ID_REQUEST;
    tx_message.extd = 0;                     // Standard frame format (11-bit ID)
    tx_message.rtr = 0;                      // Not a remote frame
    tx_message.data_length_code = 8;         // 8 bytes of data
    
    // Main loop
    while (1) {
        // Generate random data for demonstration
        for (int i = 0; i < 8; i++) {
            tx_message.data[i] = rand() % 256;
        }
        
        // Queue message for transmission
        esp_err_t result = twai_transmit(&tx_message, pdMS_TO_TICKS(1000));
        if (result == ESP_OK) {
            ESP_LOGI(TAG, "Message queued for transmission");
        } else {
            ESP_LOGE(TAG, "Failed to queue message for transmission: %s", esp_err_to_name(result));
        }
        
        // Wait before sending next message
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// Task to receive CAN bus responses
void can_response_task(void *pvParameters)
{
    twai_message_t rx_message;
    
    while (1) {
        // Wait for message to be received
        esp_err_t result = twai_receive(&rx_message, pdMS_TO_TICKS(10000));
        
        if (result == ESP_OK) {
            ESP_LOGI(TAG, "Message received - ID: 0x%03X, DLC: %d", rx_message.identifier, rx_message.data_length_code);
            
            // Print the received data
            printf("Data: ");
            for (int i = 0; i < rx_message.data_length_code; i++) {
                printf("0x%02X ", rx_message.data[i]);
            }
            printf("\n");
            
            // Process specific message IDs
            if (rx_message.identifier == CAN_ID_RESPONSE) {
                ESP_LOGI(TAG, "Received response message");
                // Process response data here
            }
        } else if (result == ESP_ERR_TIMEOUT) {
            ESP_LOGW(TAG, "Timeout waiting for message");
        } else {
            ESP_LOGE(TAG, "Failed to receive message: %s", esp_err_to_name(result));
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
}

void app_main(void)
{
    // Initialize NVS flash
    ESP_ERROR_CHECK(nvs_flash_init());
    
    // Initialize TWAI driver
    twai_init();
    
    // Create tasks for CAN communication
    xTaskCreate(can_request_task, "can_request", 4096, NULL, 5, NULL);
    xTaskCreate(can_response_task, "can_response", 4096, NULL, 5, NULL);
    
    ESP_LOGI(TAG, "CAN client started");
}