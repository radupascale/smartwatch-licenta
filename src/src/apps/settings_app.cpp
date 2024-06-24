#include "apps/settings_app.h"

/**
 * NOTE: This should be modified for further settings manually (for now..)
 */
static const char *btnmMap[] = {"Toggle WiFi", "\n", "Deep sleep",	   "\n",
								"Start SNTP",  "\n", "Configure WiFi", ""};
static const char *TAG = "SETTINGS_APP";
LV_IMAGE_DECLARE(moon);
Settings *Settings::instance = nullptr;

static SemaphoreHandle_t uiMutex;

void Settings::settings_cb(lv_event_t *e)
{
	ESP_LOGI(TAG, "Settings_cb");
	int *button = (int *)lv_event_get_param(e);
	instance->button_to_function(*button);
}

void Settings::set_wifi_config_label_text(const char *text)
{
	xSemaphoreTake(uiMutex, portMAX_DELAY);
	wifiConfigLabelText = text;
	xSemaphoreGive(uiMutex);
}

void Settings::set_wifi_label_text(const char *text)
{
	xSemaphoreTake(uiMutex, portMAX_DELAY);
	wifiStatusLabelText = text;
	xSemaphoreGive(uiMutex);
}

void Settings::set_sntp_label_text(const char *text)
{
	xSemaphoreTake(uiMutex, portMAX_DELAY);
	sntpLabelText = text;
	xSemaphoreGive(uiMutex);
}

void Settings::wifi_toggle()
{
	/* TODO: set something to signal the resumer to not change the frequency */
	wl_status_t status;
	board->set_wifi_in_use(true);

	wifiStatusLabelText = "Waiting...";
	load_screen(wifiStatusMenu);
	status = board->get_wifi_status();
	if (status == WL_CONNECTED) {
		board->wifi_stop();
		set_wifi_label_text("WiFi OFF");
		board->set_wifi_in_use(false);
	} else {
		status = board->wifi_start();
		ESP_LOGI(TAG, "Status: %d", status);
		if (status == WL_CONNECTED) {
			set_wifi_label_text("WiFi ON");
		} else {
			set_wifi_label_text("Failed!\n Configure WiFi");
			board->wifi_stop();
			board->set_wifi_in_use(false);
		}
	}

	vTaskDelay(pdMS_TO_TICKS(2000));
	restore_screen();
}

void Settings::wifi_config(void)
{
	String ip, labelText;
	board->set_wifi_in_use(true);
	/* TODO: set something to signal the resumer to not change the frequency */

	/* TODO: Change display of settings app to avoid button press */
	load_screen(wifiConfigMenu);
	/* TODO: show ip address to screen */
	ip = board->wifi_config_softap();
	labelText = "Connect to\nSSID:" + WiFi.softAPSSID() + "\n" + "http://" + ip;
	set_wifi_config_label_text(labelText.c_str());
	board->wifi_init_config_server();
	/* TODO: show finished */
	labelText = "WiFi ON";
	set_wifi_config_label_text(labelText.c_str());
	vTaskDelay(pdMS_TO_TICKS(2000));
	restore_screen();
}

void Settings::button_to_function_task(void *pvParameter)
{
	int button = *(int *)pvParameter;

	instance->button_to_function(button);
	vTaskDelete(NULL);
}

void Settings::sync_clock()
{
	load_screen(sntpMenu);
    if (board->get_sntp_status())
    {
		set_sntp_label_text("SNTP already\ninitialized");
		goto exit_sync;
    }

	if (WiFi.status() != WL_CONNECTED) {
		set_sntp_label_text("WiFi connection\nis needed!");
		goto exit_sync;
	} else {
		board->clock_init();

		int compare_year = 2024 - 1900;
		time_t now;
		struct tm timeinfo;
		time(&now);
		localtime_r(&now, &timeinfo);
		/* Hacky, but whatever*/
		while (timeinfo.tm_year < compare_year) {
			set_sntp_label_text("Waiting for system time\n to be set...");
			vTaskDelay(pdMS_TO_TICKS(2000));
			time(&now);
			localtime_r(&now, &timeinfo);
		}

		set_sntp_label_text("SNTP initialized\n successfully");
	}

exit_sync:
	vTaskDelay(pdMS_TO_TICKS(2000));
	restore_screen();
}

void Settings::button_to_function(int button)
{
	switch (button) {
	case BUTTON_1:
		wifi_toggle();
		break;
	case BUTTON_2:
		board->deep_sleep();
		break;
	case BUTTON_3:
		sync_clock();
		break;
	case BUTTON_4:
		wifi_config();
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

	lv_obj_t *icon;
	/* Init menu button matrix */
	appMenu = lv_obj_create(NULL);
	lv_scr_load(appMenu);
	icon = lv_image_create(lv_screen_active());
	lv_image_set_src(icon, &moon);
	btnm1 = lv_btnmatrix_create(lv_scr_act());
	lv_obj_set_size(btnm1, 175, 175);
	lv_obj_set_style_opa(btnm1, LV_OPA_80, 0);
	// set button matrix style to transparent
	lv_btnmatrix_set_map(btnm1, btnmMap);
	lv_btnmatrix_set_btn_ctrl(btnm1, current_button, LV_BTNMATRIX_CTRL_CHECKED);
	lv_obj_align(btnm1, LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_event_cb(btnm1, settings_cb, LV_EVENT_CLICKED, NULL);

	/* Init wifi menu */
	wifiStatusMenu = lv_obj_create(NULL);
	lv_scr_load(wifiStatusMenu);
	icon = lv_image_create(lv_screen_active());
	lv_image_set_src(icon, &moon);

	lv_obj_t *wifiStatusButton = lv_button_create(lv_screen_active());
	lv_obj_set_size(wifiStatusButton, 150, 100);
	lv_obj_align(wifiStatusButton, LV_ALIGN_CENTER, 0, 0);

	wifiStatusLabel = lv_label_create(wifiStatusButton);
	lv_label_set_text(wifiStatusLabel, "...");
	lv_obj_center(wifiStatusLabel);

	/* Init wifi config */
	wifiConfigMenu = lv_obj_create(NULL);
	lv_scr_load(wifiConfigMenu);
	icon = lv_image_create(lv_screen_active());
	lv_image_set_src(icon, &moon);

	lv_obj_t *wifiConfigButton = lv_button_create(lv_screen_active());
	lv_obj_set_size(wifiConfigButton, 150, 100);
	lv_obj_align(wifiConfigButton, LV_ALIGN_CENTER, 0, 0);

	wifiConfigLabel = lv_label_create(wifiConfigButton);
	lv_label_set_text(wifiConfigLabel, "...");
	lv_obj_center(wifiConfigLabel);

	/* Init SNTP menu */
	sntpMenu = lv_obj_create(NULL);
	lv_scr_load(sntpMenu);
	icon = lv_image_create(lv_screen_active());
	lv_image_set_src(icon, &moon);

	lv_obj_t *sntpButton = lv_button_create(lv_screen_active());
	lv_obj_set_size(sntpButton, 150, 100);
	lv_obj_align(sntpButton, LV_ALIGN_CENTER, 0, 0);

	sntpLabel = lv_label_create(sntpButton);
	lv_label_set_text(sntpLabel, "...");
	lv_obj_center(sntpLabel);

	/* Initialize ui mutex */
	uiMutex = xSemaphoreCreateBinary();
	if (uiMutex != NULL) {
		ESP_LOGI(TAG, "UI Mutex created");
		xSemaphoreGive(uiMutex);
	}

	load_screen(appMenu);
}

void Settings::restore_screen(void)
{
	if (savedScreen != nullptr) {
		xSemaphoreTake(uiMutex, portMAX_DELAY);
		currentScreen = savedScreen;
		xSemaphoreGive(uiMutex);
	}
}

void Settings::load_screen(void)
{
	xSemaphoreTake(uiMutex, portMAX_DELAY);
	lv_scr_load(currentScreen);
	xSemaphoreGive(uiMutex);
}

void Settings::load_screen(lv_obj_t *screen)
{
	xSemaphoreTake(uiMutex, portMAX_DELAY);
	savedScreen = currentScreen;
	currentScreen = screen;
	lv_scr_load(currentScreen);
	xSemaphoreGive(uiMutex);
}

void Settings::update_ui(void)
{
	lv_obj_t *activeScreen = lv_screen_active();
	bool screenMatch = activeScreen == currentScreen;

	/* bruh the naming is so confusing */
	if (!screenMatch) {
		load_screen();
		return;
	}
	if (activeScreen == wifiStatusMenu) {
		xSemaphoreTake(uiMutex, portMAX_DELAY);
		lv_label_set_text(wifiStatusLabel, wifiStatusLabelText.c_str());
		xSemaphoreGive(uiMutex);
	} else if (activeScreen == wifiConfigMenu) {
		xSemaphoreTake(uiMutex, portMAX_DELAY);
		lv_label_set_text(wifiConfigLabel, wifiConfigLabelText.c_str());
		xSemaphoreGive(uiMutex);
	} else if (activeScreen == sntpMenu) {
		xSemaphoreTake(uiMutex, portMAX_DELAY);
		lv_label_set_text(sntpLabel, sntpLabelText.c_str());
		xSemaphoreGive(uiMutex);
	}
}

void Settings::handle_button_event(uint32_t event)
{
	/**
	 * Ignore buttons for other pages, for now at least
	 */
	if (lv_screen_active() == appMenu) {
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
			xTaskCreatePinnedToCore(
				button_to_function_task, "button_to_function_task", 8096,
				(void *)&current_button, 5, NULL, APP_CPU_NUM);
			break;
		default:
			break;
		}
	}
}