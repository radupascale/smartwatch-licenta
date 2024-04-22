#include "display.h"
#include "esp_log.h"
#include <Arduino_GFX_Library.h>
#include <lvgl.h>

#define GFX_BL DF_GFX_BL

static char const *TAG = "DISPLAY";
Arduino_DataBus *bus = create_default_Arduino_DataBus();
Arduino_GFX *gfx =
	new Arduino_GC9A01(bus, DF_GFX_RST, 0 /* rotation */, true /* IPS */);
uint32_t screenWidth;
uint32_t screenHeight;
uint32_t bufSize;
lv_display_t *disp;
lv_color_t *disp_draw_buf;

uint32_t millis_cb(void)
{
	return millis();
}

/* LVGL calls it when a rendered image needs to copied to the display*/
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
	ESP_LOGI(TAG, "Flushing display");
	/*Call it to tell LVGL you are ready*/
	lv_disp_flush_ready(disp);
}

int display_init()
{
	// Init Display
	if (!gfx->begin()) {
		Serial.println("gfx->begin() failed!");
	}
	gfx->fillScreen(WHITE);

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
		ESP_LOGE(TAG, "Failed to allocate memory for display buffer");
        return -1;
	} else {
		disp = lv_display_create(screenWidth, screenHeight);
		lv_display_set_flush_cb(disp, my_disp_flush);
		lv_display_set_buffers(disp, disp_draw_buf, NULL, bufSize * 2,
							   LV_DISPLAY_RENDER_MODE_PARTIAL);

		lv_obj_t *label = lv_label_create(lv_scr_act());
		lv_label_set_text(
			label,
			"Hello Arduino, I'm LVGL!(V" GFX_STR(LVGL_VERSION_MAJOR) "." GFX_STR(
				LVGL_VERSION_MINOR) "." GFX_STR(LVGL_VERSION_PATCH) ")");
		lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
	}

	ESP_LOGI(TAG, "Display initialized");

	/* TODO: Configure LVGL task handler */
    return 0;
}