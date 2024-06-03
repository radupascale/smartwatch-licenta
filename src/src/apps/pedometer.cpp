#include "apps/pedometer.h"
#include "configs/core.h"

static const char *TAG = "PEDOMETER";

void Pedometer::count_steps()
{
	IMUData data;
	time_t timestamp;
	struct timeval tv;

	imu->read_accel();
	data = imu->get_accel_data();
	gettimeofday(&tv, NULL);

	timestamp = (tv.tv_sec * 1000 + tv.tv_usec / 1000);
	processSample(timestamp, data.x, data.y, data.z);
}

void Pedometer::reset_algorithm()
{
	resetAlgo();
}

steps_t Pedometer::get_steps()
{
	return getSteps();
}

void Pedometer::set_steps(steps_t step_count)
{
	setSteps(step_count);
}

void Pedometer::set_persistent_data(char *key, steps_t *value)
{
	board->write_to_nvs(key, (void *)value, sizeof(value), NVS_PEDOMETER_PART);
}

void *Pedometer::get_persistent_data(char *key)
{
	return board->read_from_nvs(key, sizeof(steps_t), NVS_PEDOMETER_PART);
}

void Pedometer::write_todays_steps()
{
	time_t now;
	struct tm timeinfo;
	steps_t *step_count = (steps_t*)malloc(sizeof(steps_t));
	char key[16];

    *step_count = get_steps();
	time(&now);
	localtime_r(&now, &timeinfo);

	if (timeinfo.tm_year < (2024 - 1900)) {
		ESP_LOGE(TAG,
				 "Time not properly set! Skipping writing steps to memory.");
		return;
	}
	sprintf(key, key_format, timeinfo.tm_mday, timeinfo.tm_mon + 1,
			timeinfo.tm_year + 100);
	set_persistent_data(key, step_count);

    free(step_count);
}

void Pedometer::read_todays_steps()
{
	time_t now;
	struct tm timeinfo;
	steps_t *value;
	char key[16];
	time(&now);
	localtime_r(&now, &timeinfo);

	if (timeinfo.tm_year < (2024 - 1900)) {
		ESP_LOGE(TAG,
				 "Time not properly set! Skipping reading steps from memory.");
		return;
	}

	sprintf(key, key_format, timeinfo.tm_mday, timeinfo.tm_mon + 1,
			timeinfo.tm_year + 100);
	value = (steps_t *)get_persistent_data(key);
	if (value != NULL) {
		set_steps(*value);
		free(value);
	}
}