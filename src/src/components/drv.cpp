#include "Adafruit_DRV2605.h"
#include "esp_log.h"
#include <Wire.h>

static char const *TAG = "DRV2605";
Adafruit_DRV2605 drv;

int drv_init()
{
	if (!drv.begin()) {
        ESP_LOGE(TAG, "Failed to initialize motor driver.");
	}

	drv.selectLibrary(1);
	drv.setMode(DRV2605_MODE_INTTRIG);
	ESP_LOGI(TAG, "Motor driver initialized");

    /* TODO: Configure interrupt */

    return 0;
}

void drv_play(int effect)
{
	drv.setWaveform(0, effect);
	drv.setWaveform(1, 0);

	drv.go();
}
