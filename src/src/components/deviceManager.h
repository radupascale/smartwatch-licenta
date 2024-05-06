#pragma once

/* include every component */
#include "components/bmi.h"
#include "components/display.h"
#include "components/drv.h"
#include "components/ppg.h"
#include "components/sd.h"

/**
 * @brief Class which serves as an interface between apps and components
 * 
 */
class DeviceManager
{
    private:
    Display *display;
    static DeviceManager *instance;

    public:

    DeviceManager *get_instance();
    DeviceManager();
    ~DeviceManager();
    void init();

    Display *get_display();
};