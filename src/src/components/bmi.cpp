#include "bmi.h"
#include "BMI085.h"
#include "esp_log.h"
#include "freertos/task.h"

static const char *BMI_TAG = "BMI085";

IMU::IMU()
{
    this->accel = new BMI085Accel(Wire, I2C_ACCEL);
    this->gyro = new BMI085Gyro(Wire, I2C_GYRO);
}

IMU::~IMU()
{
    delete accel;
    delete gyro;
}

int IMU::init()
{
	int status;

	status = accel->begin();
	if (status < 0) {
		ESP_LOGE(BMI_TAG, "Failed to initalize accelerometer.");
		return -1;
	}
	ESP_LOGI(BMI_TAG, "Accelerometer initialized.");

	status = gyro->begin();
	if (status < 0) {
		ESP_LOGE(BMI_TAG, "Failed to initalize gyroscope.");
		return -1;
	}
	ESP_LOGI(BMI_TAG, "Gyroscope initialized.");

	/* TODO: Configure interrupts and ODR */

	return 0;
}

void IMU::read_accel()
{
	accel->readSensor();

    accel_data.x = accel->getAccelX_mss();
    accel_data.y = accel->getAccelY_mss();
    accel_data.z = accel->getAccelZ_mss();
}

IMUData IMU::get_accel_data()
{
    return accel_data;
}