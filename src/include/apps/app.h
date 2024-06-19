#pragma once
#include "components/boardsettings.h"
#include "components/deviceManager.h"

class App
{
  public:
	DeviceManager *deviceManager;
	BoardSettings *board;
    String app_name;

	App(DeviceManager *deviceManager, String app_name)
	{
		this->deviceManager = deviceManager;
        this->app_name = app_name;

        board = deviceManager->get_board();
	}


    virtual void load_screen() = 0;
	virtual void setup_ui() = 0;
	virtual void update_ui() = 0;
};