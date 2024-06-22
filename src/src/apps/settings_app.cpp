#include "apps/settings_app.h"

/**
 * NOTE: This should be modified for further settings manually (for now..)
 */
static const char *btnmMap[] = {
	"Connect to WiFi", "\n", "Deep sleep", "\n", "setting 3", "\n", ""};
static const char *TAG = "SETTINGS_APP";
Settings *Settings::instance = nullptr;

void Settings::settings_cb(lv_event_t *e)
{
	ESP_LOGI(TAG, "Settings_cb");
	int *button = (int *)lv_event_get_param(e);
	instance->button_to_function(*button);
}

void Settings::button_to_function(int button)
{
	switch (button) {
	case BUTTON_1:
        ESP_LOGI(TAG, "Should connect to WiFi");
		break;
    case BUTTON_2:
        board->deep_sleep();
        break;
    case BUTTON_3:
        ESP_LOGI(TAG, "You are engulfed by the nothingness of space.");
        break;
	default:
		break;
	}
}

void Settings::setup_ui(void)
{
	if (setup) {
		load_screen();
		return;
	}
	setup = true;
	/* Init menu button matrix */
	appMenu = lv_obj_create(NULL);
	lv_scr_load(appMenu);
	// lv_obj_t *icon = lv_image_create(lv_screen_active());
	// lv_image_set_src(icon, &moon);
	btnm1 = lv_btnmatrix_create(lv_scr_act());
	lv_obj_set_size(btnm1, 175, 175);
	lv_obj_set_style_opa(btnm1, LV_OPA_80, 0);
	// set button matrix style to transparent
	lv_btnmatrix_set_map(btnm1, btnmMap);
	lv_btnmatrix_set_btn_ctrl(btnm1, current_button, LV_BTNMATRIX_CTRL_CHECKED);
	lv_obj_align(btnm1, LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_event_cb(btnm1, settings_cb, LV_EVENT_CLICKED, NULL);
}

void Settings::load_screen(void)
{
	lv_scr_load(appMenu);
}

void Settings::update_ui(void)
{
}

void Settings::handle_button_event(uint32_t event)
{
	switch (event) {
	case EVENT_UP:
		lv_btnmatrix_clear_btn_ctrl(btnm1, current_button,
									LV_BTNMATRIX_CTRL_CHECKED);
		current_button =
			current_button > 0 ? current_button - 1 : button_cnt - 1;
		lv_btnmatrix_set_btn_ctrl(btnm1, current_button,
								  LV_BTNMATRIX_CTRL_CHECKED);
		break;
	case EVENT_DOWN:
		lv_btnmatrix_clear_btn_ctrl(btnm1, current_button,
									LV_BTNMATRIX_CTRL_CHECKED);
		current_button = (current_button + 1) % button_cnt;
		lv_btnmatrix_set_btn_ctrl(btnm1, current_button,
								  LV_BTNMATRIX_CTRL_CHECKED);
		break;
	case EVENT_SELECT:
		// lv_obj_send_event(btnm1, LV_EVENT_CLICKED, &current_button);
        button_to_function(current_button);
		break;
	default:
		break;
	}
}