#pragma once
#include "components/deviceManager.h"

#include "apps/settings.h"

class App
{
  private:
	DeviceManager *deviceManager;
    Settings *settings;

  public:
	App(DeviceManager *deviceManager)
	{
        this->deviceManager = deviceManager;
	}

    /**
     * @brief Constructor used by apps which querry the internal state
     * of the board.
     */
    App(DeviceManager *deviceManager, Settings *settings)
    {
        this->deviceManager = deviceManager;
        this->settings = settings;
    }

	virtual void setup_ui() = 0;
	virtual void update_ui() = 0;
};