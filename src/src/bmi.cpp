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

void bmi_read()
{
	float accelX, accelY, accelZ;

	accel.readSensor();

	accelX = accel.getAccelX_mss();
	accelY = accel.getAccelY_mss();
	accelZ = accel.getAccelZ_mss();

	ESP_LOGI(TAG, "Accel: X: %f, Y: %f, Z: %f", accelX, accelY, accelZ);
}