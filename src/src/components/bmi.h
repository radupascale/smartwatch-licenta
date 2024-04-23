#pragma once

struct bmi_data_t {
	float accelX;
	float accelY;
	float accelZ;
	float gyroX;
	float gyroY;
	float gyroZ;
};

int bmi_init();
struct bmi_data_t *bmi_read();