#pragma once
#include "apps/app.h"

#define BUTTON_1 0
#define BUTTON_2 1
#define BUTTON_3 2

class Settings : public App
{
  private:
	lv_obj_t *appMenu;
    lv_obj_t *btnm1;
    int current_button = 0;
    int button_cnt = 3;

    static Settings *instance;
	void load_screen() override;
    void button_to_function(int button);
    /**
     * @brief Callback function associated with pysical
     * button presses
     * 
     * @param e 
     */
    static void settings_cb(lv_event_t *e);
  public:
	Settings(DeviceManager *deviceManager, String appName)
		: App(deviceManager, appName)
	{
        this->instance = this;
	}
	void setup_ui() override;
	void update_ui() override;
    void handle_button_event(uint32_t event);
};