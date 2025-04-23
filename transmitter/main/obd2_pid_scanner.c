#include "driver/twai.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <stdint.h>
#include <string.h>

#define PID_RANGE_SIZE 32  // Each response gives 32 PIDs (4 bytes = 32 bits)
#define MAX_SUPPORTED_PIDS 128  // Supports PIDs 0x01–0x80

static const char *TAG = "OBD2_PID_SCAN";

static uint8_t supported_pids[MAX_SUPPORTED_PIDS];
static int supported_pid_count = 0;

static bool is_pid_supported(uint8_t pid, const uint8_t *bitmap) {
    int offset = pid % 0x20;
    return (bitmap[offset / 8] >> (7 - (offset % 8))) & 1;
}

static esp_err_t send_pid_request(uint8_t pid) {
    twai_message_t req = {
        .identifier = 0x7DF,
        .flags = TWAI_MSG_FLAG_NONE,
        .data_length_code = 8,
        .data = {0x02, 0x01, pid, 0, 0, 0, 0, 0}
    };
    return twai_transmit(&req, pdMS_TO_TICKS(100));
}

static void store_supported_pid(uint8_t pid) {
    if (supported_pid_count < MAX_SUPPORTED_PIDS) {
        supported_pids[supported_pid_count++] = pid;
        ESP_LOGI(TAG, "Supported PID: 0x%02X", pid);
    }
}

const uint8_t* get_supported_pid_list(int *length) {
    if (length) *length = supported_pid_count;
    return supported_pids;
}

void get_supported_pids_task(void *arg) {
    supported_pid_count = 0;

    for (uint8_t request_pid = 0x00; request_pid <= 0x60; request_pid += 0x20) {
        ESP_LOGI(TAG, "Requesting supported PIDs from 0x%02X", request_pid);

        if (send_pid_request(request_pid) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to send PID request 0x%02X", request_pid);
            continue;
        }

        twai_message_t resp;
        if (twai_receive(&resp, pdMS_TO_TICKS(1000)) == ESP_OK) {
            if (resp.data[1] == 0x41 && resp.data[2] == request_pid) {
                ESP_LOGI(TAG, "Received bitmask for 0x%02X", request_pid);
                for (int i = 1; i <= PID_RANGE_SIZE; i++) {
                    uint8_t pid = request_pid + i;
                    if (is_pid_supported(pid, &resp.data[3])) {
                        store_supported_pid(pid);
                    }
                }
            }
        } else {
            ESP_LOGW(TAG, "No response for PID request 0x%02X", request_pid);
        }

        vTaskDelay(pdMS_TO_TICKS(300));
    }

    ESP_LOGI(TAG, "Total supported PIDs found: %d", supported_pid_count);
    vTaskDelete(NULL);
}