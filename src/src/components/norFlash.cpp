#include "components/norFlash.h"
#include "esp_log.h"

static const char *TAG = "NORFlash";

NORFlash::NORFlash()
{
}
NORFlash::~NORFlash()
{
	if (busSettings)
		delete busSettings;
}

esp_err_t NORFlash::init(void)
{
	esp_err_t ret = ESP_OK;

	ret = configure_ss_pin();
	if (ret != ESP_OK) {
		ESP_LOGI(TAG, "Failed to configure SS pin");
		return ret;
	}
	SPI.begin(BOARD_SCK, BOARD_MISO, BOARD_MOSI, SS_FLASH);
	busSettings = new SPISettings(SPI_SPEED, SPI_BYTE_ORDER_MSB, SPI_MODE0);

	return ret;
}

esp_err_t NORFlash::configure_ss_pin()
{
	gpio_config_t io_conf;
	esp_err_t err;

	io_conf.intr_type = GPIO_INTR_DISABLE;
	io_conf.mode = GPIO_MODE_OUTPUT;
	io_conf.pin_bit_mask = (1ULL << SS_FLASH);
	io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
	err = gpio_config(&io_conf);

	if (err != ESP_OK) {
		ESP_LOGI(TAG, "Failed to configure SS pin");
		return err;
	}

	return err;
}

void NORFlash::select(void)
{
	gpio_set_level(SS_FLASH, 0);
}

void NORFlash::deselect(void)
{
	gpio_set_level(SS_FLASH, 1);
}

void NORFlash::chip_reset()
{
    SPI.beginTransaction(*busSettings);
    send_instr(RST_EN);
    send_instr(RST);
    SPI.endTransaction();
}

void NORFlash::wait_busy()
{
	uint8_t sr = 0x00;

	read_status_register(READ_SRG1, &sr);
	while (sr & (1 << BUSY_BIT)) {
		read_status_register(READ_SRG1, &sr);
	}
}

void NORFlash::send_instr(uint8_t instr)
{
    select();
    SPI.transfer(instr);
    deselect();
}

void NORFlash::read_status_register(uint8_t instr, uint8_t *data)
{
	select();
	SPI.transfer(READ_SRG1);
	*data = SPI.transfer(DUMMY_VAL);
	deselect();
}

void NORFlash::chip_erase()
{
	SPI.beginTransaction(*busSettings);
    send_instr(WR_ENABLE);
    send_instr(CHIP_ERASE);
	wait_busy();
    SPI.endTransaction();
}

void NORFlash::read_data(uint32_t addr, uint8_t *buff, size_t size)
{
	uint32_t i;

	/**
	 * @brief send instruction + address
	 * then wait for SIZE bytes of data
	 *
	 */
	SPI.beginTransaction(*busSettings);
	select();
	SPI.transfer(READ_DATA);
	SPI.transfer((addr >> 16) & MASK);
	SPI.transfer((addr >> 8) & MASK);
	SPI.transfer(addr & MASK);
	for (i = 0; i < size; i++) {
		buff[i] = SPI.transfer(DUMMY_VAL);
	}
	deselect();
	SPI.endTransaction();
}

esp_err_t NORFlash::page_program(uint32_t addr, uint8_t *data, size_t size)
{
	uint32_t i;

	if (size > (FLASH_PAGE_SIZE - (addr & 0xff)))
		return ESP_ERR_INVALID_SIZE;

	SPI.beginTransaction(*busSettings);
    send_instr(WR_ENABLE);
    select();
	SPI.transfer(PAGE_PROGRAM);
	SPI.transfer((addr >> 16) & MASK);
	SPI.transfer((addr >> 8) & MASK);
	SPI.transfer(addr & MASK);
	for (i = 0; i < size; i++) {
		SPI.transfer(data[i]);
        wait_busy();
	}
	deselect();
	SPI.endTransaction();

    return ESP_OK;
}