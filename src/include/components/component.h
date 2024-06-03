#pragma once

class Component
{

  public:
	Component()
	{
	}
	/**
	 * @brief Initialize component
	 *
	 * @return 0 if the component was properly initialized, -1 on failure
	 */
	virtual int init() = 0;
};