#include "apps/settings.h"

#include "esp_attr.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_sleep.h"
#include "esp_sntp.h"
#include "esp_system.h"
#include "lwip/ip_addr.h"
#include "nvs_flash.h"
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "esp_sntp.h"

static const char *SETTINGS_TAG = "SETTINGS";
Settings *Settings::instance = nullptr;

void Settings::event_handler(void *arg, esp_event_base_t event_base,
							 int32_t event_id, void *event_data)
{
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
		esp_wifi_connect();
	} else if (event_base == WIFI_EVENT &&
			   event_id == WIFI_EVENT_STA_DISCONNECTED) {
		if (s_retry_num < MAXIMUM_RETRY) {
			esp_wifi_connect();
			s_retry_num++;
			ESP_LOGI(SETTINGS_TAG, "retry to connect to the AP");
		} else {
			xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
		}
		ESP_LOGI(SETTINGS_TAG, "connect to the AP fail");
	} else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
		ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
		ESP_LOGI(SETTINGS_TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
		s_retry_num = 0;
		xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
	}
}

void Settings::wifi_init(void)
{

	ESP_LOGI(SETTINGS_TAG, "Initializing wifi.");

	s_wifi_event_group = xEventGroupCreate();

	ESP_ERROR_CHECK(esp_netif_init());

	ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_netif_create_default_wifi_sta();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	esp_event_handler_instance_t instance_any_id;
	esp_event_handler_instance_t instance_got_ip;
	ESP_ERROR_CHECK(esp_event_handler_instance_register(
		WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler_static, NULL,
		&instance_any_id));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(
		IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler_static, NULL,
		&instance_got_ip));

	wifi_config_t wifi_config;
	memset(&wifi_config, 0, sizeof(wifi_config));
	strncpy((char *)wifi_config.sta.ssid, MY_SSID, sizeof(wifi_config.sta.ssid));
	strncpy((char *)wifi_config.sta.password, MY_PASS,
			sizeof(wifi_config.sta.password));

	wifi_config.sta.channel = 0;
	wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());

	ESP_LOGI(SETTINGS_TAG, "wifi_init_sta finished.");

	/* Waiting until either the connection is established (WIFI_CONNECTED_BIT)
	 * or connection failed for the maximum number of re-tries (WIFI_FAIL_BIT).
	 * The bits are set by event_handler() (see above) */
	EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
										   WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
										   pdFALSE, pdFALSE, portMAX_DELAY);

	/* xEventGroupWaitBits() returns the bits before the call returned, hence we
	 * can test which event actually happened. */
	if (bits & WIFI_CONNECTED_BIT) {
		ESP_LOGI(SETTINGS_TAG, "connected to ap SSID:%s password:%s", MY_SSID,
				 MY_PASS);
	} else if (bits & WIFI_FAIL_BIT) {
		ESP_LOGI(SETTINGS_TAG, "Failed to connect to SSID:%s, password:%s",
				 wifi_config.sta.ssid, wifi_config.sta.password);
	} else {
		ESP_LOGE(SETTINGS_TAG, "UNEXPECTED EVENT");
	}
}

void wifi_stop(void)
{
    esp_wifi_stop();
}

void Settings::clock_init(void)
{
	struct timeval tv;

	sntp_setservername(0, "pool.ntp.org");
	sntp_set_sync_mode(SNTP_SYNC_MODE_IMMED);
	sntp_init();
	sntp_sync_time(&tv);

	/* Set timezone to romania */
	setenv("TZ", "EET-2EEST,M3.5.0/3,M10.5.0/4", 1);
	tzset();
}

void Settings::clock_deinit(void)
{
    sntp_stop();
}