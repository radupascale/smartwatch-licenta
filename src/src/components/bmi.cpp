#include "components/bmi.h"
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

esp_err_t IMU::init()
{
	esp_err_t err = ESP_OK;

	if (accel->begin() < 0) {
		ESP_LOGE(BMI_TAG, "Failed to initalize accelerometer.");
		return ESP_FAIL;
	}
	ESP_LOGI(BMI_TAG, "Accelerometer initialized.");

	if (gyro->begin() < 0) {
		ESP_LOGE(BMI_TAG, "Failed to initalize gyroscope.");
		return ESP_FAIL;
	}
	ESP_LOGI(BMI_TAG, "Gyroscope initialized.");

	/* TODO: Configure interrupts and ODR */

    /** Accelerometer is used only for step counting => set a lower sampling rate
     * (25Hz with a 3Hz low pass)
     */
    // accel->setOdr(accel->ODR_25HZ_BW_3HZ);

	return err;
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