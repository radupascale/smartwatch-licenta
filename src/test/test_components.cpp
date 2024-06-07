/**
 * NOTE: I would have gladly split this up into multiple files IF THE
 * PLATFORMIO DOCUMENTATION was more specific as to how exactly
 * you are supposed to do that.
 */
#include "unity.h"
#include "components/bmi.h"
#include "components/drv.h"
#include "components/display.h"
#include "components/sd.h"
#include "components/norFlash.h"
#include "components/battery.h"
static DRV *drv;
static IMU *imu;
static Display *display;
static SD *sd;
static NORFlash *flash;
static Battery *battery;

void setUp(void)
{
    drv = new DRV();
    imu = new IMU();
    display = new Display();
    sd = new SD();
    flash = new NORFlash();
    battery = new Battery();
}

void tearDown(void)
{
}

/******************* FUNCTIONS COMPONENT SPECIFIC BEHAVIOUR *****************/
void test_play(void)
{
	int effect = 1;
	int count = 5;

	TEST_ASSERT_EQUAL(ESP_OK, drv->init());

	drv->enable();
	while (count) {
		drv->play(effect);
		count--;
		delay(1000);
	}
	drv->disable();
}

void test_read_accel_no_movement(void)
{
	IMUData data;
	TEST_ASSERT_EQUAL(ESP_OK, imu->init());

	imu->read_accel();
	data = imu->get_accel_data();
	TEST_ASSERT_FLOAT_WITHIN(10.0, 0, data.x);
	TEST_ASSERT_FLOAT_WITHIN(10.0, 0, data.y);
	TEST_ASSERT_FLOAT_WITHIN(10.0, 0, data.z);
}

void test_display_label(void)
{
    int i = 5;
	TEST_ASSERT_EQUAL(ESP_OK, display->init());

    display->select();
    display->enable();
	lv_obj_t *test_label = lv_label_create(lv_scr_act());
    lv_obj_align(test_label, LV_ALIGN_CENTER, 0, 0);

    while(i)
    {
        lv_label_set_text_fmt(test_label, "Closing in %ds", i);
        display->render();
        i--;
        delay(1000);
    }
    display->disable();
    display->deselect();
}

void test_sd_read_write(void)
{
    String expected = "ceva interesant";
    String actual;
    char test_file[] = "test_file";

    TEST_ASSERT_EQUAL(ESP_OK, sd->init());

    TEST_ASSERT_EQUAL(ESP_OK, sd->open_file(test_file, FILE_WRITE));
    TEST_ASSERT_GREATER_THAN(-1, sd->write_line(expected.c_str()));
    TEST_ASSERT_EQUAL(ESP_OK, sd->close_file());

    TEST_ASSERT_EQUAL(ESP_OK, sd->open_file(test_file, FILE_READ));
    actual = sd->read_line();
    TEST_ASSERT_EQUAL_MESSAGE(0, expected.compareTo(actual), expected.c_str());
    TEST_ASSERT_EQUAL(ESP_OK, sd->close_file());
}

void test_flash_read_write_byte(void)
{
	uint32_t addr = 0;
	uint8_t data = 0x69;
	uint8_t read_data = 0x00;
	TEST_ASSERT_EQUAL(ESP_OK, flash->init());
	TEST_ASSERT_EQUAL(ESP_OK, flash->page_program(addr, &data, 1));
    flash->read_data(addr, &read_data, 1);

    TEST_ASSERT_EQUAL(data, read_data);
}

void test_battery_read_voltage(void)
{
    uint32_t min_voltage = 3200;
    uint32_t max_voltage = 4200;
    uint32_t voltage = battery->read_adc();
    
    TEST_ASSERT_GREATER_THAN(min_voltage, voltage);
    TEST_ASSERT_GREATER_THAN(max_voltage, voltage);
}

/******************* FUNCTIONS FOR RUNNING TESTS *****************/

void run_unity_tests_imu(void)
{
	RUN_TEST(test_read_accel_no_movement);
}

void run_unity_tests_drv(void)
{
	RUN_TEST(test_play);
}

void run_unity_tests_display(void)
{
	RUN_TEST(test_display_label);
}

void run_unity_tests_sd(void)
{
    RUN_TEST(test_sd_read_write);
}

void run_unity_tests_flash(void)
{
    RUN_TEST(test_flash_read_write_byte);
}

void run_unity_tests_battery(void)
{
    RUN_TEST(test_battery_read_voltage);
}

int run_tests(void)
{
    UNITY_BEGIN();
	run_unity_tests_drv();
    run_unity_tests_imu();
    run_unity_tests_display();
    run_unity_tests_sd();
    run_unity_tests_flash();
    run_unity_tests_battery();
    return UNITY_END();
}

extern "C" {
void app_main()
{
    run_tests();
}
}