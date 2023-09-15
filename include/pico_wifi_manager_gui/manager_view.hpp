#include "gui/view.hpp"
#include "gui/widgets/text_widget.hpp"
#include "gui/widgets/divider_widget.hpp"
#include "gui/widgets/list_widget.hpp"

#include "pico_wifi_manager_gui/scan_view.hpp"
#include "pico_wifi_manager_gui/network_info_view.hpp"

namespace WM
{
  class WifiManagerGui : public gui::View
  {
  public:
    WifiManagerGui(View *parent);
    ~WifiManagerGui();
    static WifiManagerGui &get();
    static void init(View *parent = nullptr);
  protected:
    void handleInput(InputEvent ev) override;
  private:
    static WifiManagerGui *s_instance;

    struct {
      gui::VDividerWidget div;
      gui::TextWidget header;
      gui::ListWidget list;
      gui::TextWidget li_status, li_scan;
    } ui;
    ScanView m_scanview;

    void selectApplication(size_t idx);

  };
} // namespace WM
