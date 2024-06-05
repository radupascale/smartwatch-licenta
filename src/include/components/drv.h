#pragma once
#include "Adafruit_DRV2605.h"
#include "components/component.h"
#include <Wire.h>

class DRV : Component
{
  private:
	Adafruit_DRV2605 *drv;

  public:
	DRV();
	~DRV();

    /**
     * @brief Setup EN pin and initialize internal registers via I2C
     * 
     * @return esp_err_t ESP_OK on success
     */
	esp_err_t init() override;
    /**
     * @brief Plays the effect given as parameter.
     * The driver must be ENABLED before this function is called.
     * NOTE: Executing the following sequence doesn't yield the
     * expected results:
     *     enable();
     *     play();
     *     disable();
     * because the driver needs some time to actually control
     * the motor.
     * 
     * See datasheet for effects:
     * https://cdn-shop.adafruit.com/datasheets/DRV2605.pdf
     * 
     * @param effect index of effect in the current library (default is 1)
     */
	void play(int effect);
    void enable();
    void disable();
    esp_err_t configure_enable_pin();
};