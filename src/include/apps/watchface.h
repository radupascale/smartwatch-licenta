#pragma once
#include "apps/app.h"
#include "apps/pedometer.h"

class WatchFace : App
{
  private:
    Pedometer *pedometer = nullptr;
    Battery *battery = nullptr;

    lv_style_t symbol_style;

	lv_obj_t *time_label;
    lv_style_t time_style;
	lv_obj_t *date_label;
    lv_style_t date_style;
	lv_obj_t *steps_label;
    lv_obj_t *steps_symbol_label;
    lv_style_t steps_style;
	lv_obj_t *weather_label;
    lv_style_t weather_style;
    lv_obj_t *weather_symbol_label;
    lv_obj_t *battery_label;
    lv_style_t battery_style;
    lv_obj_t *wifi_label;
    lv_style_t wifi_style;

  public:
	WatchFace(DeviceManager *deviceManager, BoardSettings *settings) : App(deviceManager, settings)
	{
        battery = deviceManager->get_battery();
	}

    void attach_pedometer(Pedometer *pedometer);
	void setup_ui() override;
	void update_ui() override;
};