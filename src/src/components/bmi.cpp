#include "bmi.h"
#include "BMI085.h"
#include "esp_log.h"
#include "freertos/task.h"

BMI085Accel accel(Wire, 0x18);
BMI085Gyro gyro(Wire, 0x68);

static const char *TAG = "BMI085";

int bmi_init()
{
	int status;

	status = accel.begin();
	if (status < 0) {
		ESP_LOGE(TAG, "Failed to initalize accelerometer.");
		return -1;
	}
	ESP_LOGI(TAG, "Accelerometer initialized.");

	status = gyro.begin();
	if (status < 0) {
		ESP_LOGE(TAG, "Failed to initalize gyroscope.");
		return -1;
	}
	ESP_LOGI(TAG, "Gyroscope initialized.");

	/* TODO: Configure interrupts and ODR */

	return 0;
}

struct bmi_data_t *bmi_read()
{
	struct bmi_data_t *bmi_data;

	bmi_data = (struct bmi_data_t *)heap_caps_malloc(sizeof(struct bmi_data_t),
													 MALLOC_CAP_8BIT);

	accel.readSensor();
	bmi_data->accelX = accel.getAccelX_mss();
	bmi_data->accelY = accel.getAccelY_mss();
	bmi_data->accelZ = accel.getAccelZ_mss();

	gyro.readSensor();
	bmi_data->gyroX = gyro.getGyroX_rads();
	bmi_data->gyroY = gyro.getGyroY_rads();
	bmi_data->gyroZ = gyro.getGyroZ_rads();

	return bmi_data;
}