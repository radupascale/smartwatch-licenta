#include "sd.h"

#include "SdFat.h"
#include "esp_log.h"
#include <SPI.h>

SdFs sd;
FsFile file;
static char const *TAG = "SD";

int sd_init()
{
    ESP_LOGW(TAG, "Not implemented.");

	/* TODO: init SD */

	// uint32_t size, sizeMB;
	// /* Disable display before setup */
	// pinMode(TFT_SS, OUTPUT);
	// digitalWrite(TFT_SS, HIGH);

	// if (!sd.begin(SD_SS, SPI_SPEED)) {
	// 	if (sd.card()->errorCode()) {
	// 		ESP_LOGE(TAG, "SD initialization failed.");
	// 		return -1;
	// 	}
	// 	if (sd.vol()->fatType() == 0) {
	// 		ESP_LOGE(TAG, "Can't find a valid FAT16/FAT32 partition.");
	// 		return -1;
	// 	}
	// 	ESP_LOGE(TAG, "Can't determine error type");
	// 	return -1;
	// }
	// ESP_LOGI(TAG, "Card successfully initialized.");

	// /* Print card info before returning  */
	// size = sd.card()->sectorCount();

	// if (size == 0) {
	// 	ESP_LOGE(TAG, "Can't determine the card size.");
	// 	return -1;
	// }
	// sizeMB = 0.000512 * size + 0.5;
	// ESP_LOGI(TAG, "Card size: %d MB.", sizeMB);
	// ESP_LOGI(TAG, "Volume is FAT%d.", int(sd.vol()->fatType()));
	// ESP_LOGI(TAG, "Files found: ");
	// sd.ls(LS_R | LS_DATE | LS_SIZE);
	// /* TODO: Rest of initialization code */

	// /* Reenable the display as the main slave */
	// pinMode(SD_SS, OUTPUT);
	// digitalWrite(SD_SS, HIGH);
	// digitalWrite(TFT_SS, LOW);

	return 0;
}