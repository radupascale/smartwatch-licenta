#include "apps/gui.h"

// constructor


GUI::GUI()
{
	current_app = NULL;
}

static const char * btnm_map[] = {"Weather", "\n",
                                    "Settings", "\n",
                                     "...", ""};

LV_IMAGE_DECLARE(moon);
static const char *TAG = "GUI";

void lv_example_btnmatrix_1(void)
{
}

void GUI::init(App *current_app)
{
	this->current_app = current_app;

    /* Init menu button matrix */
    menu_screen = lv_obj_create(NULL);
    lv_scr_load(menu_screen);
	lv_obj_t *icon = lv_image_create(lv_screen_active());
	lv_image_set_src(icon, &moon);
    lv_obj_t *btnm1 = lv_btnmatrix_create(lv_scr_act());
    lv_obj_set_size(btnm1, 125, 125);
    lv_obj_set_style_opa(btnm1, LV_OPA_80, 0);
    // set button matrix style to transparent
    lv_btnmatrix_set_map(btnm1, btnm_map);
    lv_btnmatrix_set_btn_ctrl(btnm1, 0, LV_BTNMATRIX_CTRL_CHECKED);
    lv_obj_align(btnm1, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(btnm1, NULL, LV_EVENT_ALL, NULL);

    current_app->setup_ui();
}

void GUI::handle_event(uint32_t event)
{
    ESP_LOGI(TAG, "Event: %d", event);

    switch (event & ULONG_MAX)
    {
    case EVENT_SELECT:
        lv_scr_load(menu_screen);
        break;
    case EVENT_BACK:
        current_app->load_screen();
    default:
        break;
    }
}

void GUI::display_menu(void)
{
    lv_scr_load(menu_screen);
}

void GUI::add_app(App *app)
{
    app_list.add(app);
}

void GUI::set_current_app(App *app)
{
	current_app = app;
}

App *GUI::get_current_app(void)
{
	return current_app;
}
