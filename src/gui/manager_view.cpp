#include "pico_wifi_manager_gui/manager_view.hpp"
#include "pico_wifi_manager/pico_wifi_manager.hpp"

#include "drivers/button.hpp"
#include "buttons.hpp"

using namespace WM;

WifiManagerGui *WifiManagerGui::s_instance;

struct menu_option_t {
  const char *text;
  void (*handler)(void*);
  void *arg;
};

// menu_option_t opts[] = {
//   {"Status", }
// };

WifiManagerGui &WifiManagerGui::get()
{
  return *s_instance;
}

void WifiManagerGui::init(View *parent)
{
  if (!s_instance)
    s_instance = new WifiManagerGui(parent);
}

WifiManagerGui::WifiManagerGui(View *parent) : 
  gui::View(parent),
  m_scanview(this)
{
  printf("WifiManagerGui::WifiManagerGui()\n");
  ui.header.setText("Wifi Manager");
  ui.header.setFontSize(2);
  ui.div.addWidget(&ui.header);
  ui.div.addWidget(&ui.list);
  setWidget(&ui.div);
  ui.list.addItem(&ui.li_status);
  ui.li_status.setText("Status"); 
  ui.li_status.setFontSize(2);
  ui.li_status.setUserData(nullptr);
  ui.list.addItem(&ui.li_scan);
  ui.li_scan.setText("Scan"); 
  ui.li_scan.setFontSize(2);
  ui.li_scan.setUserData(&m_scanview);
}

WifiManagerGui::~WifiManagerGui()
{
  printf("WifiManagerGui::~WifiManagerGui()\n");
}

void WifiManagerGui::handleInput(InputEvent ev)
{
  bool handled = false;
  MyButton *b = (MyButton*)ev.data;
  switch(ev.type){
    case InputEvent::ButtonClick:{
      switch(b->get_pin()){
        case Buttons::A:{
          if(ui.list.getSelectedRow() != -1);
            selectApplication(ui.list.getSelectedRow());
          break;
        }
        case Buttons::B:{
          if(ui.list.getSelectedRow() >= 0){
            ui.list.setFocused(false);
            ui.list.setSelectedRow(-1);
          }
          else
            close();
          break;
        }
        case Buttons::X:{
          ui.list.setSelectedRow(std::max<int>(ui.list.getSelectedRow()-1, -1)); 
          ui.list.setFocused(ui.list.getSelectedRow() != -1);
          break;
        }
        case Buttons::Y:{
          ui.list.setSelectedRow(std::min<int>(ui.list.getSelectedRow()+1, ui.list.getNumItems()-1)); 
          ui.list.setFocused(ui.list.getSelectedRow() != -1);
          break;
        }
      }
      handled = true;
      break;
    }
  }
  if(!handled)
    View::handleInput(ev);
}

void WifiManagerGui::selectApplication(size_t idx)
{
  Widget *w = ui.list.getItemAt(idx);
  if (!w){
    printf("No item\n");
    return;
  }
  void *d = w->getUserData();
  if (!d){
    printf("No user data\n");
    return;
  }
  setChildView((gui::View*)d);
  auto &controller = gui::ViewController::get();
  controller.set_view((gui::View*)d);
}