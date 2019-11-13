//
// Created by patr0nus on 2019/11/13.
//

#include <utility>

#include "deskgap/app.hpp"
#include "deskgap/app.h"

extern "C" {

using DeskGap::App;
void dg_app_run(dg_app_event_callbacks event_callbacks, const void* user_data) {
    App::EventCallbacks callbacks;
    if (event_callbacks.before_quit != nullptr) {
        callbacks.onReady = [user_data, event_callbacks]() {
            event_callbacks.on_ready(user_data);
        };
        callbacks.beforeQuit = [user_data, event_callbacks]() {
            event_callbacks.before_quit(user_data);
        };
    }
    DeskGap::App::Run(std::move(callbacks));
}

}