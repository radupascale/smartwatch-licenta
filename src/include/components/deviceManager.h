#pragma once

#include "AceButton.h"
/* include every component */
#include "components/battery.h"
#include "components/bmi.h"
#include "components/boardsettings.h"
#include "components/display.h"
#include "components/drv.h"
#include "components/ppg.h"
#include "components/sd.h"

using namespace ace_button;
/**
 * @brief Class which serves as an interface between apps and components
 *
 */
class DeviceManager
{
  private:
	Display *display;
	IMU *imu;
	DRV *drv;
	BoardSettings *settings;
	Battery *battery = nullptr;

	/* Buttons */
	AceButton *button_select;
	AceButton *button_up;
	AceButton *button_down;

	static DeviceManager *instance;

  public:
	DeviceManager *get_instance();
	DeviceManager();
	~DeviceManager();
	void init(BoardSettings *settings);

	void check_buttons();
	static void handle_button_event_static(AceButton * /* button */,
										   uint8_t eventType,
										   uint8_t buttonState);
	void handle_button_event(AceButton * /* button */, uint8_t eventType,
							 uint8_t buttonState);

	Display *get_display();
	IMU *get_imu();
	DRV *get_drv();
    Battery *get_battery();
};
