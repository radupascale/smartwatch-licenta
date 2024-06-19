#include "components/deviceManager.h"
#include "apps/gui.h"

/* Singleton constructor */
DeviceManager *DeviceManager::instance = nullptr;
static char const *DEVTAG = "DEVICE_MANAGER";

DeviceManager::DeviceManager()
{
	if (this->instance != nullptr)
		return;

	this->instance = this;
}

void DeviceManager::inactivity_timer_expired(TimerHandle_t xTimer)
{
	ESP_LOGI(DEVTAG, "Timer expired");

	/* Pause all tasks in the list */
    instance->pause_tasks(); /* Maybe this should signal the resumer task to pause? */

	gpio_set_intr_type(BUTTON_SELECT, GPIO_INTR_LOW_LEVEL);
}

void DeviceManager::add_pausable_task(TaskHandle_t task)
{
    pausable_tasks.add(task);
}

void DeviceManager::pause_tasks(void)
{
    int i;

    /**
     * Use notifications because some tasks (like the render task)
     * could have actions to do before sleeping (disable the backlight)
     */
    for (i = 0; i < pausable_tasks.size(); i++)
    {
        xTaskNotify(pausable_tasks.get(i), PAUSE_TASK, eSetBits);
    }
}

void DeviceManager::resume_inactive_tasks(void)
{
    int i;

    for (i = 0; i < pausable_tasks.size(); i++)
    {
        xTaskNotify(pausable_tasks.get(i), RESUME_TASK, eSetBits);
    }
}

void DeviceManager::create_inactivity_timer(void)
{
	inactivity_timer = xTimerCreate(
		"sleep_timer", pdMS_TO_TICKS(TIMER_EXPIRE_MS), pdFALSE,
		NULL /* Timer id, not useful in this case */, inactivity_timer_expired);

	if (inactivity_timer != NULL) {
		ESP_LOGI(DEVTAG, "Timer created successfully");
	}
}

void DeviceManager::start_inactivity_timer()
{
	if (xTimerStart(inactivity_timer, 0) != pdTRUE) {
		ESP_LOGE(DEVTAG, "Failed to start timer");
	} else {
		ESP_LOGI(DEVTAG, "Timer started");
	}
}

void DeviceManager::reset_inactivity_timer()
{
	if (xTimerReset(inactivity_timer, 0) != pdTRUE) {
		ESP_LOGE(DEVTAG, "Failed to reset timer");
	} else {
		ESP_LOGI(DEVTAG, "Timer reset");
	}
}

/**
 * @brief Interrupt handler for BUTTON_SELECT to restart the timer
 *
 * @param arg NULL
 */
extern TaskHandle_t resumer_task;
static void IRAM_ATTR inactivity_isr(void *arg)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    /* Disable interrupts for this button */
	gpio_set_intr_type(BUTTON_SELECT, GPIO_INTR_DISABLE);

	vTaskNotifyGiveFromISR(resumer_task, &xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void DeviceManager::init(BoardSettings *settings)
{
	ButtonConfig *select_config, *up_config, *down_config;
	int status;

	this->settings = settings;

	/* Initialize buttons */
	select_config = new ButtonConfig();
	select_config->setEventHandler(handle_button_event_select_static);
	select_config->setFeature(ButtonConfig::kFeatureClick);
	select_config->setFeature(ButtonConfig::kFeatureDoubleClick);
	select_config->setFeature(ButtonConfig::kFeatureLongPress);
	button_select =
		new ace_button::AceButton(select_config, (uint8_t)BUTTON_SELECT);

	up_config = new ButtonConfig();
	up_config->setEventHandler(handle_button_event_up_static);
	up_config->setFeature(ButtonConfig::kFeatureClick);
	button_up = new ace_button::AceButton(up_config, (uint8_t)BUTTON_UP);
	down_config = new ButtonConfig();
	down_config->setEventHandler(handle_button_event_down_static);
	down_config->setFeature(ButtonConfig::kFeatureClick);
	button_down = new ace_button::AceButton(down_config, (uint8_t)BUTTON_DOWN);

	/* Also configure interrupt callback function for SELECT BUTTON */
	gpio_set_intr_type(BUTTON_SELECT, GPIO_INTR_DISABLE);
	gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);
	gpio_isr_handler_add(BUTTON_SELECT, inactivity_isr, NULL);
    pausable_tasks = LinkedList<TaskHandle_t>();

	/* TODO: Initialize every component */
	display = new Display();
	status = display->init();
	if (status < 0) {
		ESP_LOGE(DEVTAG, "Failed to initialize display.");
	}

	imu = new IMU();
	status = imu->init();
	if (status < 0) {
		ESP_LOGE(DEVTAG, "Failed to initialize IMU.");
	}

	drv = new DRV();
	status = drv->init();
	if (status < 0) {
		ESP_LOGE(DEVTAG, "Failed to initialize DRV.");
	}

#if USE_SD
	sd = new SD();
	status = sd->init();
	if (status < 0) {
		ESP_LOGE(DEVTAG, "Failed to initialize SD.");
	}
#endif

	/* TODO: Use this only if SD is not available OR fix shared SPI between SD
	 * and Flash */
#if !USE_SD && USE_FLASH
	flash = new NORFlash();
	status = flash->init();
	if (status < 0) {
		ESP_LOGE(DEVTAG, "Failed to initialize flash");
	}
#endif

	battery = new Battery();
	battery->init_adc();

	/* Also create the timers */
	create_inactivity_timer();
}

DeviceManager *DeviceManager::get_instance()
{
	return this->instance;
}

Display *DeviceManager::get_display()
{
	return this->display;
}

NORFlash *DeviceManager::get_flash()
{
	return this->flash;
}

SD *DeviceManager::get_sd()
{
	return this->sd;
}

IMU *DeviceManager::get_imu()
{
	return this->imu;
}

DRV *DeviceManager::get_drv()
{
	return this->drv;
}

Battery *DeviceManager::get_battery()
{
	return this->battery;
}

BoardSettings *DeviceManager::get_board()
{
	return this->settings;
}

void DeviceManager::handle_button_event_up_static(AceButton *,
												  uint8_t eventType,
												  uint8_t buttonState)
{
	instance->handle_button_event_up(nullptr, eventType, buttonState);
}

void DeviceManager::handle_button_event_up(AceButton *, uint8_t eventType,
										   uint8_t buttonState)
{
    reset_inactivity_timer();
	const __FlashStringHelper *flashStr = F(AceButton::eventName(eventType));
	char buffer[100]; // Make sure the size is large enough for your string
	strcpy_P(buffer, (PGM_P)flashStr);
	// Print out a message for all events.
	ESP_LOGI(DEVTAG, "handle event: %s", buffer);
	ESP_LOGI(DEVTAG, "button state: %d", buttonState);

	switch (eventType) {
	case AceButton::kEventPressed:
		xTaskNotify(gui_task, EVENT_UP, eSetBits);
		break;
	default:
		break;
	}
}

void DeviceManager::handle_button_event_down_static(AceButton *,
													uint8_t eventType,
													uint8_t buttonState)
{
	instance->handle_button_event_down(nullptr, eventType, buttonState);
}

void DeviceManager::handle_button_event_down(AceButton *, uint8_t eventType,
											 uint8_t buttonState)
{
    reset_inactivity_timer();
	const __FlashStringHelper *flashStr = F(AceButton::eventName(eventType));
	char buffer[100]; // Make sure the size is large enough for your string
	strcpy_P(buffer, (PGM_P)flashStr);
	// Print out a message for all events.
	ESP_LOGI(DEVTAG, "handle event: %s", buffer);
	ESP_LOGI(DEVTAG, "button state: %d", buttonState);

	switch (eventType) {
	case AceButton::kEventPressed:
		xTaskNotify(gui_task, EVENT_DOWN, eSetBits);
		settings->deep_sleep();
		break;
	default:
		break;
	}
}

void DeviceManager::handle_button_event_select_static(AceButton *,
													  uint8_t eventType,
													  uint8_t buttonState)
{
	instance->handle_button_event_select(nullptr, eventType, buttonState);
}

void DeviceManager::handle_button_event_select(AceButton *, uint8_t eventType,
											   uint8_t buttonState)
{
    reset_inactivity_timer();
	const __FlashStringHelper *flashStr = F(AceButton::eventName(eventType));
	char buffer[100]; // Make sure the size is large enough for your string
	strcpy_P(buffer, (PGM_P)flashStr);
	// Print out a message for all events.
	ESP_LOGI(DEVTAG, "handle event: %s", buffer);
	ESP_LOGI(DEVTAG, "button state: %d", buttonState);

	switch (eventType) {
	case AceButton::kEventLongPressed:
		xTaskNotify(gui_task, EVENT_SELECT, eSetBits);
		break;
	case AceButton::kEventDoubleClicked:
		xTaskNotify(gui_task, EVENT_BACK, eSetBits);
		break;
	default:
		break;
	}
}

void DeviceManager::set_gui_task(TaskHandle_t task)
{
	gui_task = task;
}

void DeviceManager::check_buttons()
{
	button_select->check();
	button_up->check();
	button_down->check();
}