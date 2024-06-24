#pragma once

#include "AceButton.h"
#include "LinkedList.h"

#include "configs/core.h"
/* include every component */
#include "components/battery.h"
#include "components/bmi.h"
#include "components/boardsettings.h"
#include "components/display.h"
#include "components/drv.h"
#include "components/norFlash.h"
#include "components/ppg.h"
#include "components/sd.h"

#define USE_DISPLAY 1
#define USE_IMU 1
#define USE_DRV 0
#define USE_SD 0
#define USE_FLASH 0

#define BUTTON_INTR_FLAG
#define TIMER_EXPIRE_MS 10000
#define PAUSE_TASK (0x31)
#define RESUME_TASK (0x30)
using namespace ace_button;
/**
 * @brief Class which serves as an interface between apps and components
 *
 */
class DeviceManager
{
  private:
	Display *display;
	IMU *imu;
	DRV *drv;
	SD *sd;
	NORFlash *flash;
	BoardSettings *settings;
	Battery *battery = nullptr;

	TaskHandle_t gui_task;
	TimerHandle_t inactivity_timer;

	/* Buttons */
	AceButton *button_select;
	AceButton *button_up;
	AceButton *button_down;

	static DeviceManager *instance;
    LinkedList<TaskHandle_t> pausable_tasks;

  public:
	DeviceManager *get_instance();
	DeviceManager();
	~DeviceManager();
	void init(BoardSettings *settings);

	void check_buttons();
	static void handle_button_event_up_static(AceButton * /* button */,
											  uint8_t eventType,
											  uint8_t buttonState);
	static void handle_button_event_down_static(AceButton * /* button */,
												uint8_t eventType,
												uint8_t buttonState);
	static void handle_button_event_select_static(AceButton * /* button */,
												  uint8_t eventType,
												  uint8_t buttonState);
	void handle_button_event_select(AceButton * /* button */, uint8_t eventtype,
									uint8_t buttonState);
	void handle_button_event_up(AceButton * /* button */, uint8_t eventtype,
								uint8_t buttonState);
	void handle_button_event_down(AceButton * /* button */, uint8_t eventtype,
								  uint8_t buttonState);
	void set_gui_task(TaskHandle_t task);

    /**
     * @brief Callback function for FreeRTOS software timer
     * NOTE: don't use prints in this function or increase
     * timer stack size if you don't want random STACK OVERFLOW errors
     * 
     * @param xTimer 
     */
	static void inactivity_timer_expired(TimerHandle_t xTimer);
	void create_inactivity_timer(void);
	void start_inactivity_timer();
	void reset_inactivity_timer();
	void pause_tasks(void);
	void resume_inactive_tasks(void);
    void add_pausable_task(TaskHandle_t task);

	NORFlash *get_flash();
	Display *get_display();
	IMU *get_imu();
	DRV *get_drv();
	Battery *get_battery();
	BoardSettings *get_board();
	SD *get_sd();
};
