#include "components/deviceManager.h"

#include "apps/watchface.h"
#include "apps/settings.h"

#include "core.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_system.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include <stdio.h>

static char const *MAIN_TAG = "MAIN";

static DeviceManager *deviceManager;
/**
 * @brief Applications
 * 
 */
static WatchFace *watch_face = nullptr;
static Settings *settings = nullptr;

void os_init()
{
    deviceManager = new DeviceManager();
    deviceManager->init();

    /* TODO: Use NVS for persistent wifi configs */
	esp_err_t ret = nvs_flash_init(); if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
		ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

    /* Initialize applications */
    settings = new Settings();
    settings->wifi_init();
    settings->clock_init();

    watch_face = new WatchFace(deviceManager, settings);
    watch_face->setup_ui();

	ESP_LOGI(MAIN_TAG, "Finish modules initialization.");
}

void os_check_buttons(void *pvParameter)
{
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(5));
        deviceManager->check_buttons();
    }
}

void os_update_display(void *pvParameter)
{
	while (1) {
        vTaskDelay(pdMS_TO_TICKS(100));
        watch_face->update_ui();
        deviceManager->get_display()->render();
	}
}


extern "C" {
void app_main()
{
	os_init();

	/* TODO: Actually create useful tasks */
	xTaskCreatePinnedToCore(&os_update_display, "os_update_display", 8096, NULL,
							5, NULL, APP_CPU_NUM);
    xTaskCreatePinnedToCore(&os_check_buttons, "os_check_buttons", 8096, NULL,
                            5, NULL, APP_CPU_NUM);
}
};
