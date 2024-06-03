#pragma once
#include "components/boardsettings.h"
#include "components/deviceManager.h"

class App
{
  public:
	DeviceManager *deviceManager;
	BoardSettings *board;
	App(DeviceManager *deviceManager)
	{
		this->deviceManager = deviceManager;
	}

	/**
	 * @brief Constructor used by apps which querry the internal state
	 * of the board.
	 */
	App(DeviceManager *deviceManager, BoardSettings *board)
	{
		this->deviceManager = deviceManager;
		this->board = board;
	}

	virtual void setup_ui() = 0;
	virtual void update_ui() = 0;
};