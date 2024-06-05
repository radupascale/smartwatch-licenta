#pragma once
#include "driver/adc.h"
#include "driver/gpio.h"

#define ADC_BAT ADC1_CHANNEL_4
#define CHG_STAT GPIO_NUM_15
#define BUTTON_SELECT GPIO_NUM_0
#define BUTTON_UP GPIO_NUM_11
#define BUTTON_DOWN GPIO_NUM_26

#define MISO 37
#define SCK 36
#define MOSI 35
#define SS_SD 3
#define SS_TFT 17
#define EN_DRV GPIO_NUM_13

#define TEST_IMU 0

#define NVS_PEDOMETER_PART "pedometer"
#define DEFAULT_NAMESPACE "default"

void os_init();