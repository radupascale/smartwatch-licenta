#include "components/battery.h"
#include "configs/core.h"
#include "esp_log.h"

static const char *TAG = "BATTERY";

Battery::Battery()
{
	min_voltage = MIN_VOLTAGE;
	max_voltage = MAX_VOLTAGE;
}

Battery::~Battery()
{
}

bool Battery::adc_calibration(void)
{
	esp_err_t ret;
	bool cali_enable = false;

	ret = esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP_FIT);
	if (ret == ESP_ERR_NOT_SUPPORTED) {
		ESP_LOGW(TAG,
				 "Calibration scheme not supported, skip software calibration");
	} else if (ret == ESP_ERR_INVALID_VERSION) {
		ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
	} else if (ret == ESP_OK) {
		cali_enable = true;
		esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11,
								 (adc_bits_width_t)ADC_WIDTH_BIT_DEFAULT, 0,
								 &adc_chars);
	} else {
		ESP_LOGE(TAG, "Invalid arg");
	}
	return cali_enable;
}

void Battery::init_adc(void)
{
	calibration_enable = adc_calibration();

	ESP_ERROR_CHECK(adc1_config_width((adc_bits_width_t)ADC_WIDTH_BIT_DEFAULT));
	ESP_ERROR_CHECK(adc1_config_channel_atten(ADC_BAT, ADC_ATTEN_DB_11));

	/* configure CHG_STAT as input */
	gpio_config_t io_conf;
	io_conf.intr_type = GPIO_INTR_DISABLE;
	io_conf.mode = GPIO_MODE_INPUT;
	io_conf.pin_bit_mask = (1ULL << CHG_STAT);
	io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
	io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
	gpio_config(&io_conf);
}

uint32_t Battery::read_adc(void)
{
	uint32_t vbat_raw, vbat = 0;

	vbat_raw = adc1_get_raw((adc1_channel_t)ADC_BAT);

	if (calibration_enable) {
		/* Multiply by two to get actual value */
		vbat = esp_adc_cal_raw_to_voltage(vbat_raw, &adc_chars) * 2;
	}
    /* TODO: No calibration? */

    return vbat;
}

uint8_t Battery::get_battery_level(void)
{
    uint32_t vbat = read_adc();

    if (vbat >= MAX_VOLTAGE)
        return MAX_CHARGED;

    return lookup_table[MAX_VOLTAGE - vbat];
}

bool Battery::is_charged(void)
{
	return gpio_get_level(CHG_STAT) == 1;
}

bool Battery::is_charging(void)
{
    return read_adc() >= MAX_VOLTAGE;
}