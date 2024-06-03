#pragma once
#include "components/component.h"
#include "BMI085.h"

#define I2C_ACCEL 0x18
#define I2C_GYRO 0x68

struct IMUData
{
    float x;
    float y;
    float z;
};

class IMU : Component
{
    private:
    BMI085Accel *accel;
    BMI085Gyro *gyro;
    IMUData accel_data;
    IMUData gyro_data;
    
    public:
    IMU();
    ~IMU();
    int init() override;
    void read_accel();
    IMUData get_accel_data();
};