#pragma once
#include "components/display.h"

class App
{
  private:
	Display *display;

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
	virtual void setup_ui() = 0;
	virtual void update_ui() = 0;
};