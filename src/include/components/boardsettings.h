#pragma once

#include "WString.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "configs/configs.h"

#include "WiFi.h"
#define MAXIMUM_RETRY 3
#define HTTP_PORT 80
#define WATCH_HOSTNAME "hector_watch"

class BoardSettings
{
  private:
    bool wifiInUse = false; /* This should have some sort of guard around it */
	void configure_deep_sleep_wakeup_source(void);
  public:
	static BoardSettings *instance;

    void set_wifi_in_use(bool value);
	BoardSettings();

	void init(void);

    void set_cpu_frequency(uint8_t mHZ);
	static void event_handler_static(void *arg, esp_event_base_t event_base,
									 int32_t event_id, void *event_data)
	{
		instance->event_handler(arg, event_base, event_id, event_data);
	}

	void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id,
					   void *event_data);

	/**
	 * @brief Configures the esp as a SoftAP, default nvs partition should be
	 * initialized before hand
	 * https://github.com/JAndrassy/lab/blob/master/ConfigurationAP/ConfigurationAP.ino
	 * @return String Ip address of the SoftAP
	 */
	String wifi_config_softap(void);
	void wifi_stop(void);
	wl_status_t wifi_start(void);
	wl_status_t get_wifi_status(void);
	/**
	 * @brief Starts a HTTP server to get ssid and pass needed for WIFI_STA.
	 * The esp has to be configured as softap beforehand
	 * https://github.com/JAndrassy/lab/blob/master/ConfigurationAP/ConfigurationAP.ino
	 */
	void wifi_init_config_server();

	/**
	 * @brief Init the system clock using SNTP (wifi connection is required)
	 *
	 */
	void clock_init(void);
	void set_timezone(void);

	/**
	 * @brief Stops the sntp server
	 *
	 */
	void clock_deinit(void);

	void deep_sleep(void);

	void init_nvs_flash(void);
	void init_pedometer_nvs_flash(void);

	/**
	 * @brief Writes a blob to NVS overwritting any existing values
	 * Reference:
	 * https://github.com/espressif/esp-idf/blob/v5.2.1/examples/storage/nvs_rw_blob/main/nvs_blob_example_main.c.
	 *
	 * @param key
	 * @param value
	 * @param size
	 * @return esp_err_t
	 */
	esp_err_t write_to_nvs(const char *key, void *value, uint32_t size,
						   const char *partition);

	/**
	 * @brief Reads blob object from NVS if it exists at the given KEY
	 * Reference
	 * https://github.com/espressif/esp-idf/blob/v5.2.1/examples/storage/nvs_rw_blob/main/nvs_blob_example_main.c.
	 *
	 * @param key
	 * @param size
	 * @return void*
	 */
	void *read_from_nvs(const char *key, uint32_t size, const char *partition);

	bool get_sntp_status(void);
	int get_boot_count(void);
};