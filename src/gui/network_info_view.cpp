#include "pico_wifi_manager_gui/network_info_view.hpp"

#include "drivers/button.hpp"
#include "buttons.hpp"

#include <cstring>

#define FONT_SIZE 2

using namespace gui;
using namespace WM;

NetworkInfoView::NetworkInfoView(View *parent, cyw43_ev_scan_result_t *network) : 
  View(parent), network(network){
    gui::Color WHITE = ViewController::get().get_default_graphics().create_pen(255, 255, 255);
    // ui.m_divider.setBox({0,0,240,135});
    ui.m_divider2.setSpacing(0);
    ui.m_divider2.setSeparator(0);
    ui.m_headerText.setText("Network Info");
    ui.m_headerText.setFontSize(FONT_SIZE);
    ui.m_headerText.setFontColor(WHITE);
    ui.m_ssidHeaderText.setText(" SSID:");
    ui.m_ssidHeaderText.setFontSize(FONT_SIZE);
    ui.m_ssidHeaderText.setFontColor(WHITE);
    ui.m_ssidBodyText.setText(std::string("  ") + std::string((char*)network->ssid, network->ssid_len));
    ui.m_ssidBodyText.setFontSize(FONT_SIZE);
    ui.m_ssidBodyText.setFontColor(WHITE);
    ui.m_bssidHeaderText.setText(" BSSID:");
    ui.m_bssidHeaderText.setFontSize(FONT_SIZE);
    ui.m_bssidHeaderText.setFontColor(WHITE);
    char buf[18];
    snprintf(buf, 17, "%02X:%02X:%02X:%02X:%02X:%02X", 
      network->bssid[0], network->bssid[1], network->bssid[2], network->bssid[3], network->bssid[4], network->bssid[5]);
    ui.m_bssidBodyText.setText(std::string("  ") + buf);
    ui.m_bssidBodyText.setFontSize(FONT_SIZE);
    ui.m_bssidBodyText.setFontColor(WHITE);
    ui.m_channelHeaderText.setText(" Channel:");
    ui.m_channelHeaderText.setFontSize(FONT_SIZE);
    ui.m_channelHeaderText.setFontColor(WHITE);
    ui.m_channelBodyText.setText(std::string("  ") + std::to_string(network->channel));
    ui.m_channelBodyText.setFontSize(FONT_SIZE);
    ui.m_channelBodyText.setFontColor(WHITE);
    ui.m_rssiHeaderText.setText(" RSSI:");
    ui.m_rssiHeaderText.setFontSize(FONT_SIZE);
    ui.m_rssiHeaderText.setFontColor(WHITE);
    ui.m_rssiBodyText.setText(std::string("  ") + std::to_string(network->rssi) + "dB");
    ui.m_rssiBodyText.setFontSize(FONT_SIZE);
    ui.m_rssiBodyText.setFontColor(WHITE);
    ui.m_divider.addWidget(&ui.m_headerText);
    ui.m_divider2.addWidget(&ui.m_ssidHeaderText);
    ui.m_divider2.addWidget(&ui.m_ssidBodyText);
    ui.m_divider2.addWidget(&ui.m_bssidHeaderText);
    ui.m_divider2.addWidget(&ui.m_bssidBodyText);
    ui.m_divider2.addWidget(&ui.m_channelHeaderText);
    ui.m_divider2.addWidget(&ui.m_channelBodyText);
    ui.m_divider2.addWidget(&ui.m_rssiHeaderText);
    ui.m_divider2.addWidget(&ui.m_rssiBodyText);
    ui.m_divider.addWidget(&ui.m_divider2);
    this->setWidget(&ui.m_divider);
  }

void NetworkInfoView::handleInput(InputEvent ev)
{
  if(ev.type == InputEvent::ButtonClick && 
     ev.data && 
     ((MyButton*)ev.data)->get_pin() == Buttons::B){
      close();
     }
  else
    View::handleInput(ev);
}
