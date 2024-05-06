#include "display.h"
#include "esp_log.h"

#define GFX_BL DF_GFX_BL

Display *Display::instance = nullptr;
static char const *DISPLAY_TAG = "DISPLAY";

uint32_t millis_cb(void)
{
	return millis();
}

Display::Display()
{
	this->instance = this;
}

void Display::flush_display(lv_display_t *disp, const lv_area_t *area,
							uint8_t *px_map)
{
	uint32_t w = lv_area_get_width(area);
	uint32_t h = lv_area_get_height(area);

	gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);
	lv_disp_flush_ready(disp);
}

int Display::init()
{
	bus = create_default_Arduino_DataBus();
	gfx = new Arduino_GC9A01(bus, DF_GFX_RST, 0 /* rotation */, true /* IPS */);

	if (!gfx->begin()) {
		Serial.println("gfx->begin() failed!");
	}
	gfx->fillScreen(BLACK);

#ifdef GFX_BL
	pinMode(GFX_BL, OUTPUT);
	digitalWrite(GFX_BL, HIGH);
#endif

	lv_init();

	/*Set a tick source so that LVGL will know how much time elapsed. */
	lv_tick_set_cb(millis_cb);

	screenWidth = gfx->width();
	screenHeight = gfx->height();

	bufSize = screenWidth * 40;

	disp_draw_buf = (lv_color_t *)heap_caps_malloc(
		bufSize * 2, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
	if (!disp_draw_buf) {
		// remove MALLOC_CAP_INTERNAL flag try again
		disp_draw_buf =
			(lv_color_t *)heap_caps_malloc(bufSize * 2, MALLOC_CAP_8BIT);
	}

	if (!disp_draw_buf) {
		ESP_LOGE(DISPLAY_TAG, "Failed to allocate memory for display buffer");
		return -1;
	} else {
		disp = lv_display_create(screenWidth, screenHeight);
		lv_display_set_flush_cb(disp, flush_display_cb);
		lv_display_set_buffers(disp, disp_draw_buf, NULL, bufSize * 2,
							   LV_DISPLAY_RENDER_MODE_PARTIAL);
	}

	ESP_LOGI(DISPLAY_TAG, "Display initialized");

	return 0;
}

void Display::render()
{
	lv_task_handler();
}

void Display::disable()
{
    digitalWrite(GFX_BL, LOW);
}

void Display::enable()
{
    digitalWrite(GFX_BL, HIGH);
}