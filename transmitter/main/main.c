#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "can_interface.h"
#include "can_reader.h"
#include "obd2_pid_scanner.h"
#include "obd2_writer.h"

void app_main(void) {
    if (!can_init()) {
        ESP_LOGE("APP", "CAN init failed");
        return;
    }

    if (!can_start()) {
        ESP_LOGE("APP", "CAN start failed");
        return;
    }

    // Start PID scan task
    xTaskCreate(get_supported_pids_task, "PID_Scanner", 4096, NULL, 10, NULL);

    // Delay to allow PID scanner to complete first scan before writer starts
    vTaskDelay(pdMS_TO_TICKS(3000));

    // Start CAN writer and reader tasks
    xTaskCreate(obd2_writer_task, "OBD2_Writer", 4096, NULL, 9, NULL);
    xTaskCreate(can_reader_task, "CAN_Reader", 4096, NULL, 10, NULL);
}