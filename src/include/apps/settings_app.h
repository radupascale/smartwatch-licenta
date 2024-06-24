#pragma once
#include "apps/app.h"

#define BUTTON_1 0
#define BUTTON_2 1
#define BUTTON_3 2
#define BUTTON_4 3

class Settings : public App
{
  private:
	lv_obj_t *appMenu;
	lv_obj_t *btnm1;
    lv_obj_t *currentScreen = nullptr;
    lv_obj_t *savedScreen = nullptr;
	int current_button = 0;
	int button_cnt = 4;

    /* Used for displaying the wifi status*/
	lv_obj_t *wifiStatusMenu;
    lv_obj_t *wifiStatusLabel;
    String wifiStatusLabelText;

    lv_obj_t *wifiConfigMenu;
    lv_obj_t *wifiConfigLabel;
    String wifiConfigLabelText;

    lv_obj_t *sntpMenu;
    lv_obj_t *sntpLabel;
    String sntpLabelText;

	static Settings *instance;
	void load_screen() override;
    void load_screen(lv_obj_t *screen);
    void restore_screen(void);
	void button_to_function(int button);
	/**
	 * @brief Callback function associated with pysical
	 * button presses
	 *
	 * @param e
	 */
	static void settings_cb(lv_event_t *e);
    static void button_to_function_task(void *pvParameter);
	void wifi_toggle();
	void wifi_config(void);
    void set_wifi_label_text(const char *text);
    void set_wifi_config_label_text(const char *text);
    void set_sntp_label_text(const char *text);
    void sync_clock();
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