#pragma once
#include "components/display.h"

#include "apps/settings.h"

class App
{
  private:
	Display *display;
    Settings *settings;

  public:
	/**
	 * @brief Construct a new App object with the default display
	 *
	 * @param display
	 */
	App(Display *display)
	{
		this->display = display;
	}

    /**
     * @brief Constructor used by apps which querry the internal state
     * of the board.
     */
    App(Display *display, Settings *settings)
    {
        this->display = display;
        this->settings = settings;
    }

	virtual void setup_ui() = 0;
	virtual void update_ui() = 0;
};