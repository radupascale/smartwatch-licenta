#pragma once
#include "apps/app.h"
#include "LinkedList.h"

#define EVENT_RENDER (0x01)
#define EVENT_STOP_RENDER (0x02)

#define EVENT_NONE (0x00)
#define EVENT_SELECT (0x01)
#define EVENT_BACK (0x02)
#define EVENT_UP (0x03)
#define EVENT_DOWN (0x04)

class GUI
{
  private:
	App *current_app;
    LinkedList<App*> app_list;
    int current_menu_button = 0;

    lv_obj_t *menu_screen;

  public:
	GUI();
	void init(App *current_app);
    void handle_event(uint32_t event);

    App *get_current_app(void);
    void set_current_app(App *app);

    void display_menu(void);
    void add_app(App *app);
};