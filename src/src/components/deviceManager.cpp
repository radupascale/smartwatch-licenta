#include "components/deviceManager.h"
#include "configs/core.h"

/* Singleton constructor */
DeviceManager *DeviceManager::instance = nullptr;
static char const *DEVTAG = "DEVICE_MANAGER";

DeviceManager::DeviceManager()
{
	if (this->instance != nullptr)
		return;

	this->instance = this;
}

void DeviceManager::init()
{
	ButtonConfig *select_config, *up_down_config;
	int status;

	/* Initialize buttons */
	select_config = new ButtonConfig();
	select_config->setEventHandler(handle_button_event_static);
	select_config->setFeature(ButtonConfig::kFeatureClick);
	select_config->setFeature(ButtonConfig::kFeatureDoubleClick);
	select_config->setFeature(ButtonConfig::kFeatureLongPress);
	select_config->setFeature(ButtonConfig::kFeatureRepeatPress);
	button_select =
		new ace_button::AceButton(select_config, (uint8_t)BUTTON_SELECT);

	/* These two buttons use the global default config */
	button_down = new ace_button::AceButton((uint8_t)BUTTON_UP);
	button_up = new ace_button::AceButton((uint8_t)BUTTON_DOWN);
	up_down_config = button_up->getButtonConfig();
	up_down_config->setEventHandler(handle_button_event_static);
	up_down_config->setFeature(ButtonConfig::kFeatureClick);

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
}

DeviceManager *DeviceManager::get_instance()
{
	return this->instance;
}

Display *DeviceManager::get_display()
{
	return this->display;
}

IMU *DeviceManager::get_imu()
{
    return this->imu;
}

DRV *DeviceManager::get_drv()
{
    return this->drv;
}

void DeviceManager::handle_button_event_static(AceButton *, uint8_t eventType,
											   uint8_t buttonState)
{
	instance->handle_button_event(nullptr, eventType, buttonState);
}

void DeviceManager::handle_button_event(AceButton *, uint8_t eventType,
										uint8_t buttonState)
{
	const __FlashStringHelper *flashStr = F(AceButton::eventName(eventType));
	char buffer[100]; // Make sure the size is large enough for your string
	strcpy_P(buffer, (PGM_P)flashStr);
	// Print out a message for all events.
	ESP_LOGI(DEVTAG, "handle event: %s", buffer);
	ESP_LOGI(DEVTAG, "button state: %d", buttonState);

	switch (eventType) {
	/**
	 * Only the select button has long press implemented
	 * in which case we turn off the display's backlight.
	 */
	case AceButton::kEventLongPressed:
		display->disable();
		break;
	case AceButton::kEventDoubleClicked:
		display->enable();
		break;
	default:
		break;
	}
}

void DeviceManager::check_buttons()
{
	button_select->check();
	button_up->check();
	button_down->check();
}