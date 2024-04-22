#include "bmi.h"
#include "core.h"
#include "display.h"
#include "driver/gpio.h"
#include "drv.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ppg.h"
#include <stdio.h>

static char const *TAG = "MAIN";

// init function pointer array
int (*init_functions[])() = {bmi_init, display_init, drv_init, ppg_init, NULL};

void os_init()
{
	int status;
	int i;

	for (i = 0; init_functions[i] != NULL; i++) {
		status = init_functions[i]();
		if (status < 0)
			vTaskDelete(NULL);
	}

	/* TODO: init SD */

	/* TODO: other things to initialize (timers, buttons, wifi, bluetooth) */

	ESP_LOGI(TAG, "Finish modules initialization.");
	vTaskDelete(NULL);
}

void os_read_bmi(void *pvParameter)
{
    while (1) {
        bmi_read();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

extern "C" {
void app_main()
{
    os_init();

    /* TODO: Actually create useful tasks */
    xTaskCreatePinnedToCore(&os_read_bmi, "os_read_bmi", 8096, NULL, 5, NULL,
                            APP_CPU_NUM);
}
};