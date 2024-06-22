#include "apps/gui.h"

static const char *TAG = "GUI";

GUI::GUI()
{
	current_app = NULL;
}

void GUI::init(App *current_app)
{
	this->current_app = current_app;
	current_app->setup_ui();
	add_app(current_app);
}

void GUI::handle_event(uint32_t event)
{
    uint32_t eventFlag = event & ULONG_MAX;

	switch (eventFlag) {
    case EVENT_SELECT: case EVENT_UP: case EVENT_DOWN:
        current_app->handle_button_event(eventFlag);
        break;
	case EVENT_LONG_SELECT:
		current_app_index = (current_app_index + 1) % app_list.size();
		change_app(current_app_index);
		break;
	case EVENT_BACK:
		current_app_index =
			current_app_index > 0 ? current_app_index - 1 : app_list.size() - 1;
        change_app(current_app_index);
        break;
	default:
		break;
	}
}

void GUI::change_app(int index)
{
    current_app = app_list.get(index);
    current_app->setup_ui();
}

void GUI::display_menu(void)
{
	ESP_LOGI(TAG, "Not implemented");
	return;
}

void GUI::add_app(App *app)
{
	app_list.add(app);
}

void GUI::set_current_app(App *app)
{
	current_app = app;
}

App *GUI::get_current_app(void)
{
	return current_app;
}
