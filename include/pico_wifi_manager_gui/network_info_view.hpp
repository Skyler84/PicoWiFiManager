#pragma once

#include "gui/view.hpp"
#include "pico/cyw43_arch.h"
#include "gui/widgets/text_widget.hpp"
#include "gui/widgets/divider_widget.hpp"

namespace WM{


    class NetworkInfoView : public gui::View{
    public:
        NetworkInfoView(View *parent, cyw43_ev_scan_result_t *network);

    protected:
        void handleInput(InputEvent ev) override;
        cyw43_ev_scan_result_t *network;
        struct UI{
            gui::VDividerWidget m_divider{nullptr, {{},{}}};
            gui::VDividerWidget m_divider2{nullptr, {{},{}}};
            gui::TextWidget m_headerText{nullptr, {}};
            gui::TextWidget m_ssidHeaderText{nullptr, {}};
            gui::TextWidget m_ssidBodyText{nullptr, {}};
            gui::TextWidget m_bssidHeaderText{nullptr, {}};
            gui::TextWidget m_bssidBodyText{nullptr, {}};
            gui::TextWidget m_channelHeaderText{nullptr, {}};
            gui::TextWidget m_channelBodyText{nullptr, {}};
            gui::TextWidget m_rssiHeaderText{nullptr, {}};
            gui::TextWidget m_rssiBodyText{nullptr, {}};
        }ui;
    };
};
