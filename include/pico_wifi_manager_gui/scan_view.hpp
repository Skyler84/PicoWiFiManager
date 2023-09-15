#pragma once

#include "gui/view.hpp"
#include "gui/view_controller.hpp"
#include "input_event.hpp"
#include "pico/cyw43_arch.h"

#include "gui/widgets/divider_widget.hpp"
#include "gui/widgets/text_widget.hpp"
#include "gui/widgets/list_widget.hpp"

namespace WM{

    class ScanView : public gui::View{
    public:
        //on create ScanView
        ScanView(View* parent);
        //on destroy ScanView
        ~ScanView(); 
    protected:
        void onEntry() override;
        void onExit() override;
        void handleInput(InputEvent ev) override;
        void onTimer(int num) override;

    private:
        int add_network(const cyw43_ev_scan_result_t *result);
        int matches_network(const cyw43_ev_scan_result_t *result);
        void clear_networks();
        void start_scan();
        void selectNetwork(int idx);
        static int scan_result(void *env, const cyw43_ev_scan_result_t *result);
    private:
        bool autoscan;
        bool scan_start;
        bool scan_in_progress;
        cyw43_ev_scan_result_t wifi_scan_results[32];
        int num_wifi_networks;
        int selected_row;
        int scroll_position = 0;
        absolute_time_t autoscan_time;

        struct UI{
            gui::VDividerWidget divider;
            gui::TextWidget headerText;
            gui::ListWidget networkList;
        }ui;
        
    };
};
