#include "pico_hal.h"
#include "pico/stdio.h"
#include "pico_wifi_manager/pico_wifi_manager.hpp"
#include "pico_wifi_manager_gui/manager_view.hpp"
#include "font6mono_data.hpp"


#include "gui/view_controller.hpp"

#include "hardware/watchdog.h"
#include "drivers/button.hpp"
#include "buttons.hpp"
#include "pico/bootrom.h"


using namespace gui;

MyButton buttons[4] = {
  MyButton(Buttons::A, Polarity::ACTIVE_LOW, 0),
  MyButton(Buttons::B, Polarity::ACTIVE_LOW, 0),
  MyButton(Buttons::X, Polarity::ACTIVE_LOW, 0),
  MyButton(Buttons::Y, Polarity::ACTIVE_LOW, 0),
};


extern "C" bool tud_connected();
void wait_stdio_usb_connected_timeout(long timeout_ms){
  if(!tud_connected())
    return;
  absolute_time_t timeout = make_timeout_time_ms(timeout_ms);

  while(!stdio_usb_connected() && absolute_time_diff_us(get_absolute_time(), timeout) > 0){
    watchdog_update();
    tight_loop_contents();
  }
  // if(stdio_usb_connected())
  sleep_ms(2000);
  watchdog_update();
}

int main()
{
  //if were connected to usb only
  if(watchdog_enable_caused_reboot())
    reset_usb_boot(6, 1);
  stdio_init_all();
  alarm_pool_init_default();
  sleep_ms(2000);
  watchdog_enable(5000, true);
  wait_stdio_usb_connected_timeout(2500);

  int ret = pico_mount(false);
  if (ret != LFS_ERR_OK)
  {
    printf("Error mounting filesystem: %d\n", ret);
    printf("Formatting...\n");
    ret = pico_mount(true);
  }
  if (ret != LFS_ERR_OK)
  {
    printf("Error formatting filesystem: %d\n", ret);
    return 1;
  }

  auto &wm = WM::WifiManager::instance();
  wm.init();

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
    wm.poll();
  }
}