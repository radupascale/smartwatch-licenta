#include "ppg.h"
#include "MAX30105.h"
#include "esp_log.h"
#include <Wire.h>

static char const *TAG = "MAX30105";
MAX30105 particleSensor;

int ppg_init()
{
	if (particleSensor.begin() == false) {
        ESP_LOGE(TAG, "Failed to initialize sensor.");
        return -1;
	}

	particleSensor.setup();
    ESP_LOGI(TAG, "Sensor initialized.");

    /* TODO: Configure interrupt */
    return 0;
}