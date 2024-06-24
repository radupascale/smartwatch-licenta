#include "components/boardsettings.h"
#include "configs/core.h"

#include "esp_attr.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_sntp.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "esp_sntp.h"
#include "esp_task_wdt.h"

static const char *TAG = "SETTINGS";
BoardSettings *BoardSettings::instance = nullptr;
RTC_DATA_ATTR static int boot_count = 0;

void BoardSettings::init(void)
{
	esp_sleep_wakeup_cause_t wakeup_cause = esp_sleep_get_wakeup_cause();

	/**
	 * This is needed to avoid KERNEL PANIC
	 * caused by the IDLE task failing to reset the WDT
	 * Some tasks take longer the default value of FreeRTOS
	 * expects.
	 */
	esp_task_wdt_init(TWDT_TIMEOUT_MS, true);

	init_nvs_flash();
	init_pedometer_nvs_flash();
    set_cpu_frequency(DEFAULT_CPU_FREQ_MHZ);
	set_timezone();

	if (wakeup_cause == ESP_SLEEP_WAKEUP_EXT0) {
		boot_count++;
	} 
}

void BoardSettings::set_wifi_in_use(bool value)
{
	wifiInUse = value;

	if (wifiInUse && getCpuFrequencyMhz() < ANTENNA_CPU_FREQ_MHZ) {
        set_cpu_frequency(ANTENNA_CPU_FREQ_MHZ);
	}
}

void BoardSettings::set_cpu_frequency(uint8_t MHz)
{
	if (MHz < ANTENNA_CPU_FREQ_MHZ && wifiInUse) {
		ESP_LOGI(TAG, "A frequency lower than 80MHz would disable WiFi");
		return;
	}
    setCpuFrequencyMhz(MHz);

    ESP_LOGI(TAG, "CPU: %u", getCpuFrequencyMhz());
}

void BoardSettings::init_nvs_flash(void)
{
	esp_err_t ret;
	/* TODO: Use NVS for persistent wifi configs */
	ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
		ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);
}

bool BoardSettings::get_sntp_status(void)
{
	return sntp_enabled();
}

int BoardSettings::get_boot_count(void)
{
	return boot_count;
}

BoardSettings::BoardSettings()
{
	if (this->instance != nullptr) {
		return;
	}
	this->instance = this;
}

void BoardSettings::wifi_init_config_server()
{
	WiFiServer configWebServer(HTTP_PORT);
	configWebServer.begin();

	while (true) {
		WiFiClient client = configWebServer.available();
		if (client) {
			char line[64];
			int l = client.readBytesUntil('\n', line, sizeof(line));
			line[l] = 0;
			client.find((char *)"\r\n\r\n");
			if (strncmp_P(line, PSTR("POST"), strlen("POST")) == 0) {
				l = client.readBytes(line, sizeof(line));
				line[l] = 0;

				/* Replace + with spaces */
				for (int i = 0; line[i]; i++) {
					if (line[i] == '+')
						line[i] = ' ';
				}

				const char *delims = "=&";
				strtok(line, delims);
				const char *ssid = strtok(NULL, delims);
				strtok(NULL, delims);
				const char *pass = strtok(NULL, delims);

				// send a response before attemting to connect to the WiFi
				// network because it will reset the SoftAP and disconnect the
				// client station
				client.println(F("HTTP/1.1 200 OK"));
				client.println(F("Connection: close"));
				client.println(
					F("Refresh: 10")); // send a request after 10 seconds
				client.println();
				client.println(F("<html><body><h3>Configuration "
								 "AP</h3><br>connecting...</body></html>"));
				client.stop();

				ESP_LOGI(TAG, "SSID: %s", ssid);
				WiFi.persistent(true);
				WiFi.setAutoReconnect(true);
				WiFi.begin(ssid, pass);
				WiFi.waitForConnectResult();
			} else {

				client.println(F("HTTP/1.1 200 OK"));
				client.println(F("Connection: close"));
				client.println();
				client.println(F("<html><body><h3>Configuration AP</h3><br>"));

				int status = WiFi.status();
				if (status == WL_CONNECTED) {
					client.println(F("Connection successful. Ending AP."));
				} else {
					client.println(
						F("<form action='/' method='POST'>WiFi connection "
						  "failed. Enter valid parameters, please.<br><br>"));
					client.println(
						F("SSID:<br><input type='text' name='i'><br>"));
					client.println(F("Password:<br><input type='password' "
									 "name='p'><br><br>"));
					client.println(
						F("<input type='submit' value='Submit'></form>"));
				}
				client.println(F("</body></html>"));
				client.stop();

				if (status == WL_CONNECTED) {
					vTaskDelay(pdMS_TO_TICKS(
						1000)); // to let the SDK finish the communication
					configWebServer.stop();
					WiFi.mode(WIFI_STA);
					return;
				}
			}
		}
	}
}

String BoardSettings::wifi_config_softap(void)
{
	IPAddress ip;
	bool status;

	status = WiFi.mode(WIFI_AP_STA);
	if (!status)
		ESP_LOGE(TAG, "Failed to configure SOFTAP");
	else
		ip = WiFi.softAPIP();

	return ip.toString();
}

void BoardSettings::wifi_stop(void)
{
	WiFi.mode(WIFI_OFF);
}

wl_status_t BoardSettings::get_wifi_status(void)
{
	return WiFi.status();
}

wl_status_t BoardSettings::wifi_start(void)
{
	wl_status_t status;

	WiFi.begin();
	/* NOTE: this blocks the current task for the duration of the function */
	WiFi.waitForConnectResult(5000);
	status = WiFi.status();

	return status;
}

void BoardSettings::configure_deep_sleep_wakeup_source(void)
{
	ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(RTC_GPIO0, 0));
}

void BoardSettings::deep_sleep(void)
{
	configure_deep_sleep_wakeup_source();
	wifi_stop();
	ESP_LOGI(TAG, "Entering deep sleep");
	esp_deep_sleep_start();
}

void BoardSettings::clock_init(void)
{
	struct timeval tv;

	sntp_setservername(0, "pool.ntp.org");
	sntp_set_sync_mode(SNTP_SYNC_MODE_IMMED);
	sntp_init();
	sntp_sync_time(&tv);

	set_timezone();
}

void BoardSettings::set_timezone(void)
{
	/* Set timezone to romania */
	setenv("TZ", "EET-2EEST,M3.5.0/3,M10.5.0/4", 1);
}

void BoardSettings::clock_deinit(void)
{
	sntp_stop();
}

void BoardSettings::init_pedometer_nvs_flash(void)
{
	esp_err_t ret = nvs_flash_init_partition(NVS_PEDOMETER_PART);

	if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
		ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase_partition(NVS_PEDOMETER_PART));
		ret = nvs_flash_init_partition(NVS_PEDOMETER_PART);
		ESP_ERROR_CHECK(ret);
		ESP_LOGI(TAG, "Return value: %s", esp_err_to_name(ret));
	}
	ESP_LOGI(TAG, "NVS partition %s initialized", NVS_PEDOMETER_PART);
}

esp_err_t BoardSettings::write_to_nvs(const char *key, void *value,
									  uint32_t size, const char *partition)
{
	nvs_handle_t handler;
	esp_err_t err = ESP_OK;

	err = nvs_open_from_partition(partition, DEFAULT_NAMESPACE, NVS_READWRITE,
								  &handler);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Error (%s) opening NVS handle", esp_err_to_name(err));
		return err;
	}

	/* This will overwrite the value at the nvs location if it exists */
	err = nvs_set_blob(handler, key, value, size);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Error (%s) writing to NVS", esp_err_to_name(err));
		goto exit;
	}

	err = nvs_commit(handler);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Error (%s) flashing to NVS", esp_err_to_name(err));
	}

exit:
	nvs_close(handler);

	ESP_LOGI(TAG, "Finish writing to NVS");
	return err;
}

void *BoardSettings::read_from_nvs(const char *key, uint32_t size,
								   const char *partition)
{
	nvs_handle_t handler;
	esp_err_t err;
	uint32_t len;
	void *value = NULL;

	err = nvs_open_from_partition(partition, DEFAULT_NAMESPACE, NVS_READWRITE,
								  &handler);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Error (%s) opening NVS handle", esp_err_to_name(err));
	}

	err = nvs_get_blob(handler, key, NULL, &len);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Error (%s) reading from NVS handle",
				 esp_err_to_name(err));
		goto exit;
	}

	if (len == 0) {
		ESP_LOGI(TAG, "No value set at key %s in NVS partition %s", key,
				 partition);
	} else {
		value = malloc(len);
		err = nvs_get_blob(handler, key, value, &len);
		if (err != ESP_OK) {
			ESP_LOGE(TAG, "Error (%s) reading from NVS handle",
					 esp_err_to_name(err));
			free(value);
		}
	}

exit:
	nvs_close(handler);
	ESP_LOGI(TAG, "Finish reading from NVS");
	return value;
}