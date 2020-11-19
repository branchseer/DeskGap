#ifndef DESKGAP_BROWSER_WINDOW_HPP
#define DESKGAP_BROWSER_WINDOW_HPP

#include <string>
#include <array>
#include <vector>
#include <functional>
#include <optional>
#include "menu.hpp"
#include "webview.hpp"

namespace DeskGap {

    class BrowserWindow {
    private:
        friend class Dialog;
        struct Impl;
        std::unique_ptr<Impl> impl_;
    public:
        struct EventCallbacks {
            std::function<void()> onBlur;
            std::function<void()> onFocus;
            std::function<void()> onResize;
            std::function<void()> onMove;
            std::function<void()> onClose;
#ifdef __APPLE__
            std::function<void()> willEnterFullScreen;
            std::function<void()> didEnterFullScreen;
            std::function<void()> willExitFullScreen;
            std::function<void()> didExitFullScreen;
#endif
        };
        explicit BrowserWindow(const WebView&, EventCallbacks&&);
        BrowserWindow(const BrowserWindow&) = delete;

        void SetMaximizable(bool);
        void SetMinimizable(bool);
        void SetResizable(bool);
        void SetHasFrame(bool);
        void SetClosable(bool);

        void Minimize();

        void Show();
        void Center();

        void Destroy();
        void Close();

        void SetTitle(const std::string& utf8title);

        void SetSize(int width, int height, bool animate);
        void SetPosition(int x, int y, bool animate);
   
        std::array<int, 2> GetSize();
        std::array<int, 2> GetPosition();

        void SetMaximumSize(int width, int height);
        void SetMinimumSize(int width, int height);

    #ifndef __APPLE__
        void SetMenu(const Menu*);
        void SetIcon(const std::optional<std::string>& iconPath);
    #endif

    #ifdef __APPLE__
        enum class TitleBarStyle: int {
            DEFAULT = 0,
            HIDDEN = 1,
            HIDDEN_INSET = 2
        };
        void SetTitleBarStyle(TitleBarStyle);
        void SetTrafficLightPosition(int x, int y);

        struct Vibrancy {
            std::string material;
            std::string blendingMode;
            std::string state;
            struct Constraint {
                std::string attribute;
                double value;
                enum class Unit { POINT, PERCENTAGE };
                Unit valueUnit;
            };
            std::array<Constraint, 4> constraints;
        };
        void SetVibrancies(const std::vector<Vibrancy>&);
    #endif

        void PopupMenu(const Menu&, const std::array<int, 2>* location, int positioningItem, std::function<void()>&& onClose);
        ~BrowserWindow();
    };
}

#endif
