#pragma once
#include "driver/adc.h"
#include "driver/gpio.h"

#define ADC_BAT ADC1_CHANNEL_4
#define CHG_STAT GPIO_NUM_15
#define BUTTON_SELECT GPIO_NUM_0
#define BUTTON_UP GPIO_NUM_11
#define BUTTON_DOWN GPIO_NUM_26
#define RTC_GPIO0 BUTTON_UP

#define BOARD_MISO 37
#define BOARD_SCK 36
#define BOARD_MOSI 35
#define SS_SD 3
#define SS_TFT 17
#define SS_FLASH GPIO_NUM_14
#define EN_DRV GPIO_NUM_13

#define SPI_SPEED (5000000UL)

#define NVS_PEDOMETER_PART "pedometer"
#define DEFAULT_NAMESPACE "default"

#define TWDT_TIMEOUT_MS 3000
#define DEFAULT_CPU_FREQ_MHZ 40
#define ANTENNA_CPU_FREQ_MHZ 80 // wifi and bluetooth only works at this freq
#define INACTIVE_CPU_FREQ_MHZ 20

/* App and GUI notifications */
#define EVENT_NONE (0x00)
#define EVENT_LONG_SELECT (0x01)
#define EVENT_BACK (0x02)
#define EVENT_UP (0x03)
#define EVENT_DOWN (0x04)
#define EVENT_SELECT (0x05)

void os_init(void *pvParam);
void os_gui(void *pvParam);
void os_resumer(void *pvParam);