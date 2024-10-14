#include <stdint.h>
#include <stdio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "nvs_flash.h"

// TODO: 
// - [ ] Clean up the code to functions, like intializing the wifi
// - [ ] Maybe add some type of main loop for. ex. every 5 minutes loop for APs

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

void app_main(void)
{
    char* taskName = pcTaskGetName(NULL);
    ESP_LOGI(taskName, "Setting up"); // Log that we are on setup

    ESP_ERROR_CHECK(nvs_flash_init());

    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&config));
    // NOTE: Default wifi mode is station mode so no need to set it here
    // ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_LOGI(taskName, "Wifi initialized!");
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(taskName, "Wifi started!");

    const wifi_scan_config_t scan_config = {
        .ssid = 0,
        .bssid = 0,
        .channel = 0,
        .show_hidden = true,
    };

    ESP_LOGI(taskName, "Set up done. Starting scan now!"); // Log that we have done our setup
    // Scan wifi APs
    ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));

    ESP_LOGI(taskName, "Scan done. Getting scan info now!");

    uint16_t num_of_aps;
    esp_wifi_scan_get_ap_num(&num_of_aps); // Store the number of found APs to num_of_aps

    wifi_ap_record_t ap_recs[num_of_aps];
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
}
