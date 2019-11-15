//
// Created by Wang, Chi on 2019/11/15.
//

#ifndef DESKGAP_APP_STARTUP_HPP
#define DESKGAP_APP_STARTUP_HPP

#include <string>
#include <vector>
#include <deskgap/app.hpp>

namespace DeskGap {
    class AppStartup {
    public:
        static const std::vector<std::string>& ExecArgs();
        static const std::string& ResourcePath();
        static void SignalAppRun(App::EventCallbacks&&);
    };
}

#endif //DESKGAP_APP_STARTUP_HPP
