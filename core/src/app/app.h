#ifndef app_h
#define app_h

#include <functional>
#include <memory>
#include "../menu/menu.h"
#include <string>

namespace DeskGap {
    class App {
    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    public:
        struct EventCallbacks {
            std::function<void()> onReady;
            std::function<void()> beforeQuit;
        };
        App(EventCallbacks&& callbacks);

        void Run();
        void Exit(int exitCode);

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
        void SetMenu(std::optional<std::reference_wrapper<Menu>> menu);
    #endif

        std::string GetPath(PathName name);
        ~App();
    };
}

#endif
