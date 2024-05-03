#include "components/bmi.h"
#include "components/display.h"
#include "components/drv.h"
#include "components/ppg.h"

#include "apps/watchface.h"

#include "core.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_system.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include <stdio.h>

static char const *TAG = "MAIN";

// init function pointer array
int (*init_functions[])() = {bmi_init, drv_init, ppg_init, NULL};

static Display display;

static WatchFace *watch_face = nullptr;

void os_init()
{
	int status;
	int i;

    status = display.init();
    if (status < 0) 
    {
        ESP_LOGE(TAG, "Failed to initialize display.");
    }

	for (i = 0; init_functions[i] != NULL; i++) {
		status = init_functions[i]();
		if (status < 0) {
			ESP_LOGE(TAG, "Failed to initialize module %d.", i);
		}
	}

	/* TODO: init SD */

    /* Initialize applications */
    watch_face = new WatchFace(&display);
    watch_face->setup_ui();

	/* TODO: other things to initialize (timers, buttons, wifi, bluetooth) */

	ESP_LOGI(TAG, "Finish modules initialization.");
}

void os_update_display(void *pvParameter)
{
	while (1) {
		vTaskDelay(pdMS_TO_TICKS(10));

        display.render();
	}
}

extern "C" {
void app_main()
{
	os_init();

	/* TODO: Actually create useful tasks */
	xTaskCreatePinnedToCore(&os_update_display, "os_update_display", 8096, NULL,
							5, NULL, APP_CPU_NUM);
}
};