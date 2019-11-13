#ifndef DESKGAP_APP_HPP
#define DESKGAP_APP_HPP

#include <functional>
#include <memory>
#include "menu.hpp"
#include <string>

namespace DeskGap {
    class App {
    public:
        struct EventCallbacks {
            std::function<void()> onReady;
            std::function<void()> beforeQuit;
        };

        static void Run(EventCallbacks&& callbacks);
        static void Exit(int exitCode);

        enum class PathName: uint32_t {
            APP_DATA = 0,
            TEMP = 1,
            DESKTOP = 2,
            DOCUMENTS = 3,
            DOWNLOADS = 4,
            MUSIC = 5,
            PICTURES = 6,
            VIDEOS = 7,
            HOME = 8,
        };
    #ifdef __APPLE__
        static void SetMenu(std::optional<std::reference_wrapper<Menu>> menu);
    #endif
        static std::string GetPath(PathName name);
        static std::string GetResourcePath(const char* argv0);
    };
}

#endif
