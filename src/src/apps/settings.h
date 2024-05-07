#pragma once

#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include <string.h>

#include "lwip/err.h"
#include "lwip/sys.h"

#include "configs/configs.h"

#define MAXIMUM_RETRY 3

/* The event group allows multiple bits for each event, but we only care
about
 * two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1


class Settings
{
  private:
	EventGroupHandle_t s_wifi_event_group;
	int s_retry_num = 0;

  public:
	static Settings *instance;

	Settings()
	{
        if (this->instance != nullptr)
        {
            return;
        }
        this->instance = this;       
	}

    static void event_handler_static(void *arg, esp_event_base_t event_base,
                                        int32_t event_id, void *event_data)
    {
        instance->event_handler(arg, event_base, event_id, event_data);
    }

	void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id,
					   void *event_data);

    /**
     * @brief Init wifi using the wifi STATION example from ESP
     * 
     */
    void wifi_init(void);

    /**
     * @brief Init the system clock using SNTP (wifi connection is required)
     * 
     */
    void clock_init(void);
};