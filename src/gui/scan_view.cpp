#include "pico_wifi_manager_gui/scan_view.hpp"
#include "drivers/button.hpp"
#include "buttons.hpp"
#include "pico_wifi_manager_gui/network_info_view.hpp"

#include "pico_graphics.hpp"

#include "gui/widgets/text_widget.hpp"

#include "pico/critical_section.h"

#define FONT_SCALE 2
#define TEXT_HEIGHT_PX (5*FONT_SCALE)
#define TEXT_SPACING_PX (1*FONT_SCALE)
#define HEADER_SPACING_PX (3*FONT_SCALE)
#define TEXT_ROW_PX (TEXT_HEIGHT_PX+2*TEXT_SPACING_PX)
#define HEADER_PX (TEXT_HEIGHT_PX+2*HEADER_SPACING_PX)

#define DEBUG_PRINTF(...) printf("[SCANVIEW] " __VA_ARGS__)

using namespace gui;
using namespace WM;

ScanView::ScanView(View *parent): View(parent){
  setWidget(&ui.divider);
  ui.divider.addWidget(&ui.headerText);
  ui.divider.addWidget(&ui.networkList);
  printf("e\n");
  ui.networkList.setFocused(true);
  ui.headerText.setFontSize(3);
  ui.headerText.setText("WifiScan:");
  ui.networkList.setSize({0, 135});
  DEBUG_PRINTF("constructed\n");
}
ScanView::~ScanView(){}

void ScanView::onEntry(){
  DEBUG_PRINTF("onEntry\n");
  this->autoscan = true;
  ui.networkList.setSelectedRow(-1);
  start_scan();
}
void ScanView::onExit(){
  this->autoscan = false;
  // cancel
}

void ScanView::handleInput(InputEvent ev){
  bool redraw = true;
  MyButton *b = (MyButton*)ev.data;
  DEBUG_PRINTF("Input Event %d (%p)\n", ev.type, b);
  switch(ev.type){
    case InputEvent::ButtonClick:{
      switch(b->get_pin()){
        case Buttons::A:{
          if(this->selected_row == -1)
            this->scan_start = true;
          else
            selectNetwork(selected_row);
            ;
          break;
        }
        case Buttons::B:{
          if(this->selected_row >= 0){
            ui.networkList.setFocused(false);
            this->selected_row = -1;
          }
          else
            close();
          break;
        }
        case Buttons::X:{
          DEBUG_PRINTF("X clicked\n");
          this->selected_row = std::max(this->selected_row-1, -1); 
          ui.networkList.setSelectedRow(this->selected_row);
          ui.networkList.setFocused(this->selected_row >= 0);
          break;
        }
        case Buttons::Y:{
          DEBUG_PRINTF("Y clicked\n");
          this->selected_row = std::min(this->selected_row+1, this->num_wifi_networks-1); 
          ui.networkList.setSelectedRow(this->selected_row);
          ui.networkList.setFocused(this->selected_row >= 0);
          break;
        }
        default: redraw = false;
      }
      break;
    }
    case InputEvent::ButtonHold:{
      switch(b->get_pin()){
        case Buttons::A:
          if(selected_row == -1)
            autoscan = !autoscan;
            start_timer(5'000'000);
          break;
        default: redraw = false;
      }
      break;
    }
    default: redraw = false;
  }
  if(redraw)
    requiresRedraw();
}

void ScanView::onTimer(int num){
  DEBUG_PRINTF("Timer %d\n", num);
  if(cyw43_wifi_scan_active(&cyw43_state))
    start_timer(100'000); //100ms in us
  else if(scan_in_progress){
    scan_in_progress = 0;
    start_timer(1'000'000);
    autoscan_time = make_timeout_time_ms(10'000);
  } else if(scan_start || autoscan && absolute_time_diff_us(get_absolute_time(), autoscan_time) < 0)
    start_scan();
}

int ScanView::add_network(const cyw43_ev_scan_result_t *result){
  //ignore hidden networks
  if((!result->ssid[0] && false /*hide_hidden_nets*/) || num_wifi_networks >= 32)
    return -1;
  char buf[32];
  memcpy(&this->wifi_scan_results[num_wifi_networks], result, sizeof(*result));
  gui::TextWidget *w = new gui::TextWidget;
  w->setFontSize(2);
  if (result->ssid[0])
    snprintf(buf, 31, "% 2d %-10.10s%c% 2d", num_wifi_networks, (char*)result->ssid, result->ssid_len>10?'.':' ', result->channel);
  else
    snprintf(buf, 31, "% 2d ...:%02x:%02x:%02x % 2d", num_wifi_networks, result->bssid[3], result->bssid[4], result->bssid[5], result->channel);
  w->setText(buf);
  w->setUserData(&this->wifi_scan_results[num_wifi_networks]);
  ui.networkList.addItem(w);
  //need to dispatch to eventHandler?
  return num_wifi_networks++;
}

int ScanView::matches_network(const cyw43_ev_scan_result_t *result){
  for(int i = 0; i < this->num_wifi_networks; i++)
    if(memcmp(result->bssid, this->wifi_scan_results[i].bssid, 6) == 0)
      return i;
  return -1;
}

int ScanView::scan_result(void *env, const cyw43_ev_scan_result_t *result){
  assert(env);
  DEBUG_PRINTF("Got result\n");
  ScanView *view = (ScanView*)env;
  if (!result)
    return 0;
  if (int idx = view->matches_network(result); idx >= 0)
    return 0;
  view->add_network(result);
  DEBUG_PRINTF("ssid: %-32s rssi: %4d chan: %3d mac: %02x:%02x:%02x:%02x:%02x:%02x sec: %u\n",
      result->ssid, result->rssi, result->channel,
      result->bssid[0], result->bssid[1], result->bssid[2], result->bssid[3], result->bssid[4], result->bssid[5],
      result->auth_mode);
  return 0;
}

void ScanView::start_scan(){
  //just double check
  if(cyw43_wifi_scan_active(&cyw43_state)){
    // start_timer(10'000'000);
    DEBUG_PRINTF("Scan already active\n");
    return;
  }
  DEBUG_PRINTF("Starting scan\n");
  cyw43_wifi_scan_options_t scan_options = {0};
  scan_start = false;
  DEBUG_PRINTF("Scan state: %p\n", &cyw43_state);
  int err = cyw43_wifi_scan(&cyw43_state, &scan_options, this, scan_result);
  if (err == 0) {
    DEBUG_PRINTF("Performing wifi scan\n");
    requiresRedraw();

    scan_in_progress = true;
    //clear list
    num_wifi_networks = 0;
    while(gui::Widget *w = ui.networkList.removeItemAt(0)){
      DEBUG_PRINTF("Deleting (%p)\n", w);
      delete w;
    }
    selected_row = -1;
    start_timer(1'000'000);
    return;
  } else {
    DEBUG_PRINTF("Failed to start scan: %d\n", err);
    start_timer(10'000'000); // wait 10s and scan again
    return;
  }
}

void ScanView::selectNetwork(int idx){
  if(idx >= num_wifi_networks)
    return;
  setChildView(new NetworkInfoView(this, &wifi_scan_results[idx]));
  ViewController::get().set_view(getChildView());

}
