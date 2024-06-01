#include "esp_log.h"
#include "components/drv.h"

static char const *TAG = "DRV2605";

int DRV::init()
{
	if (!drv->begin()) {
        ESP_LOGE(TAG, "Failed to initialize motor driver.");
        return -1;
	}

	drv->selectLibrary(1);
	drv->setMode(DRV2605_MODE_INTTRIG);
	ESP_LOGI(TAG, "Motor driver initialized");

    /* TODO: Configure interrupt */

    return 0;
}

void DRV::play(int effect)
{
    ESP_LOGI(TAG, "Playing effect %d", effect);
	drv->setWaveform(0, effect);
	drv->setWaveform(1, 0);

	drv->go();
}

DRV::DRV()
{
    this->drv = new Adafruit_DRV2605();
}

DRV::~DRV()
{
    delete drv;
}