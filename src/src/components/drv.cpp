#include "components/drv.h"
#include "configs/core.h"
#include "esp_log.h"

static char const *TAG = "DRV2605";

esp_err_t DRV::init()
{
    esp_err_t err = ESP_OK;

	/* Set the ENABLE GPIO pin as output and the drv to LOW */
    err = configure_enable_pin();
	if(err != ESP_OK)
    {
		ESP_LOGE(TAG, "Failed to initialize motor driver.");
		return err;
    }
    
	enable();

	if (!drv->begin(&Wire)) {
		ESP_LOGE(TAG, "Failed to initialize motor driver.");
		return ESP_FAIL;
	}

	drv->selectLibrary(1);
	drv->setMode(DRV2605_MODE_INTTRIG);

	/* TODO: Configure interrupt */

	/* Disable the device when not in use */
	disable();

	ESP_LOGI(TAG, "Motor driver initialized");
	return err;
}

void DRV::play(int effect)
{
	ESP_LOGI(TAG, "Playing effect %d", effect);
	drv->setWaveform(0, effect);
	drv->setWaveform(1, 0);

	drv->go();
}

esp_err_t DRV::configure_enable_pin()
{
	gpio_config_t io_conf;
	esp_err_t err;

	io_conf.intr_type = GPIO_INTR_DISABLE;
	io_conf.mode = GPIO_MODE_OUTPUT;
	io_conf.pin_bit_mask = (1ULL << EN_DRV);
	io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
	err = gpio_config(&io_conf);

	if (err != ESP_OK) {
		ESP_LOGI(TAG, "Failed to configure EN pin");
		return err;
	}

	return err;
}

void DRV::enable()
{
	gpio_set_level(EN_DRV, HIGH);
}

void DRV::disable()
{
	gpio_set_level(EN_DRV, LOW);
}

DRV::DRV()
{
	this->drv = new Adafruit_DRV2605();
}

DRV::~DRV()
{
	delete drv;
}