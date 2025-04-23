#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/twai.h"
#include "esp_log.h"
#include "obd2_pid_scanner.h"

static const char *TAG = "OBD2_WRITER";

void obd2_writer_task(void *arg) {
    int pid_count = 0;
    const uint8_t *pid_list = get_supported_pid_list(&pid_count);

    if (pid_count == 0 || pid_list == NULL) {
        ESP_LOGW(TAG, "No supported PIDs available to request");
        vTaskDelete(NULL);
        return;
    }

    while (1) {
        for (int i = 0; i < pid_count; i++) {
            twai_message_t msg = {
                .identifier = 0x7DF,
                .flags = TWAI_MSG_FLAG_NONE,
                .data_length_code = 8,
                .data = {0x02, 0x01, pid_list[i], 0, 0, 0, 0, 0}
            };

            esp_err_t err = twai_transmit(&msg, pdMS_TO_TICKS(100));
            if (err == ESP_OK) {
                ESP_LOGI(TAG, "Requested PID: 0x%02X", pid_list[i]);
            } else {
                ESP_LOGE(TAG, "Transmit failed for PID 0x%02X: %s", pid_list[i], esp_err_to_name(err));
            }

            vTaskDelay(pdMS_TO_TICKS(250));  // delay between each PID request
        }

        vTaskDelay(pdMS_TO_TICKS(3000));  // wait before next full PID cycle
    }
}