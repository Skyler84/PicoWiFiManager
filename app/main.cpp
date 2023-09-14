#include "pico_hal.h"
#include "pico/stdio.h"
#include "pico_wifi_manager.hpp"
#include "pico_wifi_manager_gui.hpp"
#include "font6mono_data.hpp"


#include "gui/view_controller.hpp"

#include "hardware/watchdog.h"
#include "drivers/button.hpp"
#include "buttons.hpp"

using namespace gui;

MyButton buttons[4] = {
  MyButton(Buttons::A, Polarity::ACTIVE_LOW, 0),
  MyButton(Buttons::B, Polarity::ACTIVE_LOW, 0),
  MyButton(Buttons::X, Polarity::ACTIVE_LOW, 0),
  MyButton(Buttons::Y, Polarity::ACTIVE_LOW, 0),
};


int main()
{
  stdio_init_all();

  // int ret = pico_mount(false);
  // if (ret != LFS_ERR_OK)
  // {
  //   printf("Error mounting filesystem: %d\n", ret);
  //   printf("Formatting...\n");
  //   ret = pico_mount(true);
  // }
  // if (ret != LFS_ERR_OK)
  // {
  //   printf("Error formatting filesystem: %d\n", ret);
  //   return 1;
  // }

  MyButton::init_all();
  auto &controller = ViewController::get();
  WM::WifiManagerGui::init(nullptr);
  auto &v = WM::WifiManagerGui::get();
  v.setBackgroundColor(controller.get_default_graphics().create_pen(120, 40, 60));
  controller.set_view(&v);
  controller.get_default_graphics().set_font(&font6mono);
  controller.get_default_display().set_backlight(100);

  while(true) {
    controller.poll_event_loop();
    watchdog_update();
  }
}