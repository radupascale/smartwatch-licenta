#include "apps/watchface.h"
#include "apps/pedometer.h"
#include "components/boardsettings.h"

#include "configs/core.h"
#include "driver/adc.h"
#include "driver/gpio.h"
#include "esp_adc_cal.h"
#include "esp_log.h"
#include "esp_system.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include <stdio.h>

static char const *MAIN_TAG = "MAIN";

static DeviceManager *deviceManager = nullptr;
static BoardSettings *settings = nullptr;
static WatchFace *watch_face = nullptr;
static Pedometer *pedometer = nullptr;

void wait_for_sntp()
{
	/* Get the current time and date */
	int retry = 0;
	time_t now;
	struct tm timeinfo;
	time(&now);
	localtime_r(&now, &timeinfo);

	/* Hacky, but whatever*/
	while (timeinfo.tm_year < (2024 - 1900) && retry < MAX_TIME_RETRY) {
		ESP_LOGI(MAIN_TAG, "Waiting for system time to be set...");
		vTaskDelay(2000 / portTICK_PERIOD_MS);
		time(&now);
		localtime_r(&now, &timeinfo);
		retry++;
	}
}

void os_init()
{
    esp_err_t ret;

	settings = new BoardSettings();
    settings->init_pedometer_nvs_flash();

	deviceManager = new DeviceManager();
	deviceManager->init(settings);

	/* TODO: Use NVS for persistent wifi configs */
	ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
		ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	/* Initialize applications */
	settings->wifi_init();
	settings->clock_init();
    wait_for_sntp();

    pedometer = new Pedometer(deviceManager, settings);

	watch_face = new WatchFace(deviceManager, settings);
    watch_face->attach_pedometer(pedometer);
	watch_face->setup_ui();

	ESP_LOGI(MAIN_TAG, "Finish modules initialization.");
}

void os_update_display(void *pvParameter)
{
	while (1) {
		vTaskDelay(pdMS_TO_TICKS(200));
		watch_face->update_ui();
		deviceManager->get_display()->render();
	}
}

void os_pedometer(void *pvParameter)
{
    steps_t step_count;
    steps_t last_step_count = pedometer->get_steps();
    steps_t delta = 10;
    pedometer->reset_algorithm();

	while (1) {
        pedometer->count_steps();

        step_count = pedometer->get_steps();

        if (step_count - last_step_count > delta)
        {
            last_step_count = step_count;
            pedometer->write_todays_steps();
        }
		vTaskDelay(pdMS_TO_TICKS(20));
	}
}

/**
 * @brief Task which checks button events every 5ms (as mentioned in the
 * ACEButton library README)
 *
 * @param pvParameter
 */
void os_check_buttons(void *pvParameter)
{
	while (1) {
		vTaskDelay(pdMS_TO_TICKS(5));
		deviceManager->check_buttons();
	}
}


extern "C" {
void app_main()
{
	os_init();

	xTaskCreatePinnedToCore(&os_update_display, "os_update_display", 8096, NULL,
							5, NULL, APP_CPU_NUM);
	xTaskCreatePinnedToCore(&os_check_buttons, "os_check_buttons", 8096, NULL,
							5, NULL, APP_CPU_NUM);
	xTaskCreatePinnedToCore(&os_pedometer, "os_pedometer", 8096, NULL, 5, NULL,
							APP_CPU_NUM);
#if TEST_IMU
	xTaskCreatePinnedToCore(&os_toggle_imu, "os_toggle_imu", 8096, NULL, 5,
							NULL, APP_CPU_NUM);
#endif
}
};
