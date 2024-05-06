#include "components/deviceManager.h"

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
    int status;
    int i;

    /* TODO: Initialize every component */

    display = new Display();
    status = display->init();
    if (status < 0)
    {
        ESP_LOGE(DEVTAG, "Failed to initialize display.");
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
