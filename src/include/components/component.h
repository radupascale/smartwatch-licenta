#pragma once
#include "esp_err.h"

class Component
{

  public:
	Component()
	{
	}
	/**
	 * @brief Initialize component
	 *
	 * @return ESP_OK on success
	 */
	virtual esp_err_t init() = 0;
};