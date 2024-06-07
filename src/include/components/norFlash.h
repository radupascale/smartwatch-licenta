#pragma once
#include "SPI.h"
#include "components/component.h"
#include "configs/core.h"

#define SPI_BYTE_ORDER_MSB 1

#define FLASH_PAGE_SIZE (256)
/**
 * @brief SPI instruction codes from the datasheet:
 * https://www.winbond.com/resource-files/W25Q256JW%20SPI%20RevJ%2003102021%20Plus.pdf
 *
 */

#define READ_SRG1 (0x05)
#define WR_ENABLE (0x06) // set before any PAGE_PROGRAM, ERASE, or WRITE to SR
#define READ_DATA (0x03)
#define PAGE_PROGRAM (0x02) // write data?
#define SECTOR_ERASE (0x20) // sector = 4KB
#define BLOCK_ERASE_32KB (0x52)
#define BLOCK_ERASE_64KB (0x52)
#define CHIP_ERASE (0x07)
#define RST_EN (0x66)
#define RST (0x99)

#define BUSY_BIT 0

#define DUMMY_VAL (0x00)
#define MASK (0xff)

class NORFlash : Component
{
  private:
	SPISettings *busSettings = nullptr;

  public:
	/**
	 * @brief This uses the default SPI bus in SPI.cpp
	 * By default, this flash uses 3 byte addresses (set by
	 * ADP = 0 in one of the status registers)
	 *
	 * @return esp_err_t
	 */
	esp_err_t init(void) override;
	NORFlash();
	~NORFlash();
	esp_err_t configure_ss_pin(void);
	void select(void);
	void deselect(void);
	void send_instr(uint8_t instr);
	/* SPI operations for W25Q256 module */
	void chip_reset();
	void read_status_register(uint8_t instr, uint8_t *data);
	/**
	 * @brief BUSY bit has to be low before any transactions
	 *
	 */
	void wait_busy();
	void read_data(uint32_t addr, uint8_t *buff, size_t size);
	/**
	 * @brief Implements the page_program instruction from the datasheet
	 * write_enable -> send_address -> send_data -> end
	 * NOTE: If an entire 256 byte page is to be programmed, the last address
	 * byte (the 8 least significant address bits) should be set to 0.
	 * This is why this function returns an error if the given size
	 * would overwrite data from the current page
	 * @param addr
	 * @param data
	 * @param size
	 */
	esp_err_t page_program(uint32_t addr, uint8_t *data, size_t size);
	void chip_erase();
};