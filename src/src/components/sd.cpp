#include "components/sd.h"
#include "configs/core.h"
#include "esp_log.h"

static char const *TAG = "SD";

SD::SD()
{
}

SD::~SD()
{
}

esp_err_t SD::init()
{

#ifdef SS_SD
	pinMode(SS_SD, OUTPUT);
#endif

	SPI.begin(SCK, MISO, MOSI, SS_SD);
	if (!sd.begin(SS_SD)) {
		if (sd.card()->errorCode()) {
			ESP_LOGE(TAG, "SD initialization failed.\n");
			return ESP_FAIL;
		}
	}

	ESP_LOGI(TAG, "Card successfully initialized.");
	return ESP_OK;
}

esp_err_t SD::open_file(char *filename, int mode)
{
	if (opened) {
		ESP_LOGE(TAG, "Another file is already opened");
		return ESP_FAIL;
	}

	file = sd.open(filename, mode);
	opened = 1;
	return ESP_OK;
}

esp_err_t SD::close_file()
{
	if (!opened) {
		ESP_LOGE(TAG, "No file to close");
		return ESP_FAIL;
	}

	file.close();
	opened = 0;
	return ESP_OK;
}

int SD::write_line(const char *buff)
{
	return file.println(buff);
}

String SD::read_line()
{
	String buff = file.readStringUntil('\n');

	return buff;
}

void SD::select()
{
	digitalWrite(SS_SD, LOW);
}

void SD::deselect()
{
	digitalWrite(SS_SD, HIGH);
}
