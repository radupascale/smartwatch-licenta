#pragma once
#include "components/deviceManager.h"
#include "components/boardsettings.h"

class App
{
  private:
	DeviceManager *deviceManager;
    BoardSettings *settings;

  public:
	App(DeviceManager *deviceManager)
	{
        this->deviceManager = deviceManager;
	}

    /**
     * @brief Constructor used by apps which querry the internal state
     * of the board.
     */
    App(DeviceManager *deviceManager, BoardSettings *settings)
    {
        this->deviceManager = deviceManager;
        this->settings = settings;
    }

	virtual void setup_ui() = 0;
	virtual void update_ui() = 0;
};