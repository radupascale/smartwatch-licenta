#include "apps/gui.h"
#include "apps/pedometer.h"
#include "apps/settings_app.h"
#include "apps/watchface.h"
#include "components/boardsettings.h"

#include "configs/core.h"
#include "driver/adc.h"
#include "driver/gpio.h"
#include "esp_adc_cal.h"
#include "esp_log.h"
#include "esp_system.h"

#include "esp32/clk.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include <stdio.h>

static char const *MAIN_TAG = "MAIN";

static DeviceManager *deviceManager = nullptr;
static BoardSettings *settings = nullptr;
static WatchFace *watchFace = nullptr;
static Settings *settingsApp = nullptr;
static Pedometer *pedometer = nullptr;

TaskHandle_t gui_task;
TaskHandle_t display_task;
TaskHandle_t resumer_task;
TaskHandle_t button_task;
static SemaphoreHandle_t lvgl_mutex;
static GUI *gui = nullptr;

void os_init()
{
	/* Initialize LVGL mutex */
	lvgl_mutex = xSemaphoreCreateBinary();
	if (lvgl_mutex != NULL) {
		ESP_LOGI(MAIN_TAG, "LVGL Mutex created");
		xSemaphoreGive(lvgl_mutex);
	}

	/* TODO: call it board */
	settings = new BoardSettings();
	settings->init();

	deviceManager = new DeviceManager();
	deviceManager->init(settings);

	/* Initialize applications */
	/* TODO: why do these also get settings as an argument?  */
	pedometer = new Pedometer(deviceManager, "Pedometer");
	watchFace = new WatchFace(deviceManager, "Watchface");
	watchFace->attach_pedometer(pedometer);
	settingsApp = new Settings(deviceManager, "Settings");

	/* TODO: init gui */
	gui = new GUI();
	gui->init(watchFace);
	/* TODO: add applications */
	gui->add_app(settingsApp);

	ESP_LOGI(MAIN_TAG, "Finish modules initialization. Boot count: %d",
			 settings->get_boot_count());
}

void os_update_display(void *pvParameter)
{
	Display *display = deviceManager->get_display();
	uint32_t ulNotifiedValue = 0x00;

	while (1) {
		xTaskNotifyWait(0, 0, &ulNotifiedValue, pdMS_TO_TICKS(200));

		if (ulNotifiedValue & PAUSE_TASK) {
			ESP_LOGI(MAIN_TAG, "Pause Update display");
			display->disable();

			/* Note: the event it is waiting for is RESUME_TASK */
			xTaskNotifyWait(ULONG_MAX, ULONG_MAX, &ulNotifiedValue,
							portMAX_DELAY);
			display->enable();
			ESP_LOGI(MAIN_TAG, "Resume update display");
		}
		/**
		 * LVGL is not thread safe, use this to guard display update
		 * operations, see this: https://docs.lvgl.io/8.3/porting/os.html
		 */
		xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
		gui->get_current_app()->update_ui();
		display->render();
		xSemaphoreGive(lvgl_mutex);
	}
}

void os_gui(void *pvParameter)
{
	uint32_t ulNotifiedValue = 0x00;

	while (1) {
		xTaskNotifyWait(ULONG_MAX, ULONG_MAX, &ulNotifiedValue, portMAX_DELAY);

		// lock mutex
		xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
		gui->handle_event(ulNotifiedValue);
		xSemaphoreGive(lvgl_mutex);
	}
}

void os_pedometer(void *pvParameter)
{
	steps_t step_count;
	steps_t last_step_count = pedometer->get_steps();
	steps_t delta = 10;
	pedometer->reset_algorithm();

	while (1) {
		pedometer->count_steps();

		step_count = pedometer->get_steps();

		if (step_count - last_step_count > delta) {
			last_step_count = step_count;
			pedometer->write_todays_steps();
		}
		vTaskDelay(pdMS_TO_TICKS(20));
	}
}

/**
 * @brief Task which checks button events every 5ms (as mentioned in the
 * ACEButton library README)
 *
 * @param pvParameter
 */
void os_check_buttons(void *pvParameter)
{
	uint32_t ulNotifiedValue;

	while (1) {
		xTaskNotifyWait(0, 0, &ulNotifiedValue, pdMS_TO_TICKS(5));

		if (ulNotifiedValue & PAUSE_TASK) {
			ESP_LOGI(MAIN_TAG, "Pausing button task");
			/* Note: the event it is waiting for is RESUME_TASK */
			xTaskNotifyWait(ULONG_MAX, ULONG_MAX, &ulNotifiedValue,
							portMAX_DELAY);
			ESP_LOGI(MAIN_TAG, "Resuming button task");
			/* Prevent reading of button press after waking up */
			vTaskDelay(pdMS_TO_TICKS(200));
		}
		deviceManager->check_buttons();
	}
}

/**
 * @brief Task who's sole purpose is to PAUSE / RESUME
 * tasks which get suspended when the inactivity timer
 * expires
 *
 * @param pvParameter
 */
void os_resumer(void *pvParameter)
{
	deviceManager->add_pausable_task(display_task);
	deviceManager->add_pausable_task(button_task);
	deviceManager->start_inactivity_timer();

	/* TODO: Change frequency based on wifi status */
	int pause = 1;
	while (1) {
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if (pause) {
			ESP_LOGI(MAIN_TAG, "Pausing tasks");
			deviceManager->pause_tasks();
            settings->set_cpu_frequency(INACTIVE_CPU_FREQ_MHZ);
			pause = 0;
		} else {
            settings->set_cpu_frequency(DEFAULT_CPU_FREQ_MHZ);
			deviceManager->resume_inactive_tasks();
			ESP_LOGI(MAIN_TAG, "Resuming tasks");
			deviceManager->reset_inactivity_timer();
			pause = 1;
		}
	}
}

#ifndef UNIT_TEST
extern "C" {
void app_main()
{
	os_init();

	xTaskCreatePinnedToCore(&os_update_display, "os_update_display", 8096, NULL,
							5, &display_task, APP_CPU_NUM);
	xTaskCreatePinnedToCore(&os_gui, "os_gui", 8096, NULL, 6, &gui_task,
							APP_CPU_NUM);
	deviceManager->set_gui_task(gui_task);

	xTaskCreatePinnedToCore(&os_check_buttons, "os_check_buttons", 8096, NULL,
							5, &button_task, APP_CPU_NUM);
	xTaskCreatePinnedToCore(&os_pedometer, "os_pedometer", 8096, NULL, 5, NULL,
							APP_CPU_NUM);
	xTaskCreatePinnedToCore(&os_resumer, "os_resumer", 8096, NULL, 7,
							&resumer_task, APP_CPU_NUM);
}
};
#endif