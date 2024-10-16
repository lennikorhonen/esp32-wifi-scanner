#include <stdint.h>
#include <stdio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "nvs_flash.h"
#include "portmacro.h"

#define BLINK_LED 2

char* auth_mode_to_char(wifi_auth_mode_t mode) {
    char *modes[] = {
        "OPEN",
        "WEP",
        "WPA_PSK",
        "WPA2_PSK",
        "WPA WPA_PSK2",
        "EAP",
        "WPA2_EAP",
        "WPA3_PSK",
        "WAPI_PSK",
        "OWE",
        "WPA3_ENT_192",
        "WP3_EXT_PSK",
        "WPA3_EXT_PSK_MIXED",
        "MAX"
    };
    return modes[mode];
}

void wifi_init() {
    char* taskName = pcTaskGetName(NULL);
    ESP_ERROR_CHECK(nvs_flash_init());

    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&config));
    // NOTE: Default wifi mode is station mode so no need to set it here
    // ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_LOGI(taskName, "Wifi initialized!");
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(taskName, "Wifi started!");
}

void app_main(void)
{
    char* taskName = pcTaskGetName(NULL);
    ESP_LOGI(taskName, "Setting up"); // Log that we are on setup

    wifi_init();

    const wifi_scan_config_t scan_config = {
        .ssid = 0,
        .bssid = 0,
        .channel = 0,
        .show_hidden = true,
    };

    gpio_reset_pin(BLINK_LED);
    gpio_set_direction(BLINK_LED, GPIO_MODE_OUTPUT);

    ESP_LOGI(taskName, "Set up done. Starting scan now!"); // Log that we have done our setup

    // main loop
    for (;;) {
        gpio_set_level(BLINK_LED, 1);
        // Scan wifi APs
        ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));
        ESP_LOGI(taskName, "Scan done. Getting scan info now!");
        gpio_set_level(BLINK_LED, 0);

        uint16_t num_of_aps; // variable to save the number of found APs
        esp_wifi_scan_get_ap_num(&num_of_aps); // Store the number of found APs to num_of_aps

        wifi_ap_record_t ap_recs[num_of_aps]; // Variable to save the found AP records with length of found APs
        esp_wifi_scan_get_ap_records(&num_of_aps, ap_recs); // Store the found AP records to ap_recs

        if (num_of_aps == 0) {
            printf("Didn't find any APs\n");
        } else {
            printf("Found %d access points\n", num_of_aps);
            printf("\n");
            printf(" SSID                 | Channel | Signal strength | Auth mode \n");
            printf("==============================================================\n");
            for (int i = 0; i < num_of_aps; i++) { // Main loop
                // Log wifi APs
                printf(" %20s | %7d | %15d | %9s\n", (char *)ap_recs[i].ssid, ap_recs[i].primary, ap_recs[i].rssi, auth_mode_to_char(ap_recs[i].authmode));
            }
            printf("==============================================================\n");
        }

        vTaskDelay(300000 / portTICK_PERIOD_MS); // Delay for 5 minute between each scan
    }
}
