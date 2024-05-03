#include "apps/watchface.h"
#include "resources/fonts/symbols.h"

LV_FONT_DECLARE(digital_7_72px);
LV_FONT_DECLARE(digital_7_24px);
LV_FONT_DECLARE(custom_symbols_14px);

/**
 * @brief Initialize labels for time, steps and weather.
 * 
 */
void WatchFace::setup_ui()
{
    /* Used style only for displaying symbols */
    lv_style_init(&symbol_style);
    lv_style_set_text_font(&symbol_style, &custom_symbols_14px); 

    lv_style_init(&time_style); 
    lv_style_set_text_font(&time_style, &digital_7_72px);
    time_label = lv_label_create(lv_scr_act());
    lv_label_set_text(time_label, "00:00");
    lv_obj_add_style(time_label, &time_style, 0);
    lv_obj_align(time_label, LV_ALIGN_CENTER, 0, -20);

    lv_style_init(&date_style);
    lv_style_set_text_font(&date_style, &digital_7_24px);
    date_label = lv_label_create(lv_scr_act());
    lv_label_set_text(date_label, "FR, 3 MAY");
    lv_obj_add_style(date_label, &date_style, 0);
    lv_obj_align_to(date_label, time_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

    lv_style_init(&steps_style);
    lv_style_set_text_font(&steps_style, &lv_font_montserrat_20);
    steps_label = lv_label_create(lv_scr_act());
    lv_label_set_text(steps_label, "123");
    lv_obj_add_style(steps_label, &steps_style, 0);
    lv_obj_align(steps_label, LV_ALIGN_BOTTOM_MID, 40, -20);

    steps_symbol_label = lv_label_create(lv_scr_act());
    lv_obj_add_style(steps_symbol_label, &symbol_style, 0);
    lv_label_set_text(steps_symbol_label, STEP_SYMBOL);
    lv_obj_align_to(steps_symbol_label, steps_label, LV_ALIGN_OUT_LEFT_MID, 0, 0);

    /* Dummy weather */
    lv_style_init(&weather_style);
    lv_style_set_text_font(&weather_style, &lv_font_montserrat_20);
    weather_label = lv_label_create(lv_scr_act());
    lv_label_set_text(weather_label, "14°C");
    lv_obj_add_style(weather_label, &weather_style, 0);
    lv_obj_align_to(weather_label, steps_symbol_label, LV_ALIGN_OUT_LEFT_MID, -10, 0);

    weather_symbol_label = lv_label_create(lv_scr_act());
    lv_obj_add_style(weather_symbol_label, &symbol_style, 0);
    lv_label_set_text(weather_symbol_label, SUNNY_SYMBOL);
    lv_obj_align_to(weather_symbol_label, weather_label, LV_ALIGN_OUT_LEFT_MID, 0, 0);

    lv_style_init(&battery_style);
    lv_style_set_text_font(&battery_style, &lv_font_montserrat_20);
    battery_label = lv_label_create(lv_scr_act());
    lv_label_set_text(battery_label, LV_SYMBOL_BATTERY_1);
    lv_obj_add_style(battery_label, &battery_style, 0);
    lv_obj_align(battery_label, LV_ALIGN_TOP_MID, -20, 20);

    lv_style_init(&wifi_style);
    lv_style_set_text_font(&wifi_style, &lv_font_montserrat_20);
    wifi_label = lv_label_create(lv_scr_act());
    lv_label_set_text(wifi_label, LV_SYMBOL_WIFI);
    lv_obj_add_style(wifi_label, &wifi_style, 0);
    lv_obj_align_to(wifi_label, battery_label, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

}

void WatchFace::update_ui()
{
    return;
}