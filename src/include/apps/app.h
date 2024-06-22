#pragma once
#include "components/boardsettings.h"
#include "components/deviceManager.h"

class App
{
  protected:
	DeviceManager *deviceManager;
	BoardSettings *board;
	String app_name;
	bool setup = false;

	App(DeviceManager *deviceManager, String appName)
	{
		this->deviceManager = deviceManager;
		this->app_name = appName;

		board = deviceManager->get_board();
	}
	virtual void load_screen() = 0;

  public:
    virtual void handle_button_event(uint32_t event) = 0;
	virtual void setup_ui() = 0;
	virtual void update_ui() = 0;
};