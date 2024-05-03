#pragma once
#include "apps/app.h"

class WatchFace : App
{
  private:
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
	WatchFace(Display *display) : App(display)
	{
	}

	void handle_event(void *event) override
	{
		return;
	}
	void setup_ui() override;
	void update_ui() override;
};