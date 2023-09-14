#include "pico_wifi_manager_gui.hpp"

using namespace WM;

WifiManagerGui *WifiManagerGui::s_instance;

WifiManagerGui &WifiManagerGui::get()
{
  return *s_instance;
}

void WifiManagerGui::init(View *parent)
{
  if (!s_instance)
    s_instance = new WifiManagerGui(parent);
}

WifiManagerGui::WifiManagerGui(View *parent) : gui::View(parent)
{
  printf("WifiManagerGui::WifiManagerGui()\n");
  ui.header.setText("Wifi Manager");
  ui.header.setFontSize(2);
  ui.div.addWidget(&ui.header);
  ui.div.addWidget(&ui.list);
  setWidget(&ui.div);
}

WifiManagerGui::~WifiManagerGui()
{
  printf("WifiManagerGui::~WifiManagerGui()\n");
}

void WifiManagerGui::handleInput(InputEvent ev)
{
  this->gui::View::handleInput(ev);
  // gui::View::handleInput(ev);
}