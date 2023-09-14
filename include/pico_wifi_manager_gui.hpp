#include "gui/view.hpp"
#include "pico_wifi_manager.hpp"
#include "gui/widgets/text_widget.hpp"
#include "gui/widgets/divider_widget.hpp"
#include "gui/widgets/list_widget.hpp"

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
    } ui;
  };
} // namespace WM
