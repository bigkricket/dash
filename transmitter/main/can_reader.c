#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/twai.h"
#include "esp_log.h"

static const char *TAG = "CAN_READER";

void can_reader_task(void *arg) {
    twai_message_t msg;

    while (1) {
        esp_err_t result = twai_receive(&msg, portMAX_DELAY);

        if (result == ESP_OK) {
            if (msg.rtr || msg.data_length_code < 3) {
                continue;
            }

            // Log raw message
            ESP_LOGI(TAG, "Received: ID=0x%03lX, DLC=%d", msg.identifier, msg.data_length_code);

            for (int i = 0; i < msg.data_length_code; i++) {
                printf("%02X ", msg.data[i]);
            }
            printf("\n");
            

        } else {
            ESP_LOGW(TAG, "Receive error: %s", esp_err_to_name(result));
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}