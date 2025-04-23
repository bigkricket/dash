#include "driver/gpio.h"
#include "driver/twai.h"
#include "esp_log.h"

#define CAN_RX GPIO_NUM_21
#define CAN_TX GPIO_NUM_22

static const char *TAG = "CAN";


bool can_init(void) {
    // Initialize configuration structures using macro initializer
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(CAN_RX, CAN_TX, TWAI_MODE_NORMAL);

    // Sets Baudrate
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();

    // Sets filters on COBIDs
    twai_filter_config_t f_config = {
        .acceptance_code = (0x7E8 << 21),
        .acceptance_mask = ~(0x7FF << 21),
        .single_filter = true
    };

    // Install TWAI driver 
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
        ESP_LOGI(TAG, "Driver installed\n");
        return true;
    } else {
        ESP_LOGE(TAG, "Failed to install driver\n");
        printf("Failed to install driver\n");
        return false;
    }
}

bool can_start(void) {
    // Start TWAI driver
    if (twai_start() == ESP_OK) {
        ESP_LOGI(TAG, "Driver started\n");
        return true;
    } else {
        ESP_LOGE(TAG, "Driver could not start\n");
        return false;
    }
}

bool can_stop(void) {
    esp_err_t err = twai_stop();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Uninstall failed %s\n", esp_err_to_name(err));
        return false;
    }

    err = twai_driver_uninstall();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Uninstall failed %s\n", esp_err_to_name(err));
        return false;
    }

    ESP_LOGI(TAG, "Uninstaled and stopped driver");
    return true;
}