#pragma once
#include "component.h"
#include "components/bmi.h"
#include <Arduino_GFX_Library.h>
#include <lvgl.h>

class Display : Component
{
  private:
	Arduino_DataBus *bus;
	Arduino_GFX *gfx;
	uint32_t screenWidth;
	uint32_t screenHeight;
	uint32_t bufSize;
	lv_display_t *disp;
	lv_color_t *disp_draw_buf;

  public:
	static Display *instance;

	Display();
    ~Display();

	/**
	 * @brief LVGL calls it when a rendered image needs to copied to the display
	 *
	 * @param disp
	 * @param area
	 * @param px_map
	 */
	void flush_display(lv_display_t *disp, const lv_area_t *area,
					   uint8_t *px_map);

	/**
	 * @brief Callback function of lvgl must not be a class method so
	 * we use a static method which calls the relevant function
	 *
	 * @param disp
	 * @param area
	 * @param px_map
	 */
	static void flush_display_cb(lv_display_t *disp, const lv_area_t *area,
								 uint8_t *px_map)
	{
		instance->flush_display(disp, area, px_map);
	}
	void render();
    esp_err_t init() override;

    void disable();
    void enable();
    void select();
    void deselect();
};