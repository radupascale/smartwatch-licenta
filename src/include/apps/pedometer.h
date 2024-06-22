#pragma once

#include "StepCountingAlgo.h"
#include "apps/app.h"
#include "components/bmi.h"

class Pedometer : public App
{
  private:
	IMU *imu;
	const char *key_format = "%02d%02d%02d";

  public:
	Pedometer(DeviceManager *deviceManager, String app_name)
		: App(deviceManager, app_name)
	{
		imu = deviceManager->get_imu();
        if (imu != NULL)
            initAlgo();

		/* Try to read the step count from nvs */
		read_todays_steps();
	}

	~Pedometer()
	{
	}

	void setup_ui() override
	{
	}

	void update_ui() override
	{
	}

    void load_screen() override
    {
        
    }

    void handle_button_event(uint32_t event) override
    {

    }

	void reset_algorithm();
	steps_t get_steps();
	void set_steps(steps_t step_count);
	/**
	 * @brief Filters, processes and analyzes accelerometer data to determine if
	 * a step was taken.
	 *
	 */
	void count_steps();
	void set_persistent_data(char *key, steps_t *value);
	void *get_persistent_data(char *key);

	/**
	 * @brief Set the number of steps taken today up to this point to
	 * non-volatile memory
	 *
	 */
	void write_todays_steps();

	/**
	 * @brief Get the number of steps taken today from non-volatile memory and
	 * update the step count
	 *
	 */
	void read_todays_steps();
};