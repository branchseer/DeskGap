#include <cstdlib>
#include <shlobj_core.h>
#include <fileapi.h>
#include "./util/wstring_utf8.h"
#include <filesystem>
#include "app.hpp"
#include "webview_impl.h"
#include "dispatch_wnd.hpp"

namespace DeskGap {
    HWND dispatchWindowWnd;
    void App::Init() {
        OleInitialize(nullptr);

        const wchar_t* const DispatcherWndClassName = L"DeskGapDispatcherWindow";

        WNDCLASSEXW dispatcherWindowClass { };
        dispatcherWindowClass.cbSize = sizeof(WNDCLASSEXW);
        dispatcherWindowClass.hInstance = GetModuleHandleW(nullptr);
        dispatcherWindowClass.lpszClassName = DispatcherWndClassName;
        dispatcherWindowClass.lpfnWndProc = [](HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) -> LRESULT {
            if (msg == DG_DISPATCH_MSG) {
                auto action = reinterpret_cast<std::function<void()>*>(lp);
                (*action)();
                delete action;
                return 0;
            }
            else {
                return DefWindowProcW(hwnd, msg, wp, lp);
            }
        };
        RegisterClassExW(&dispatcherWindowClass);

        dispatchWindowWnd = CreateWindowW(
                DispatcherWndClassName,
                L"",
                0, 0, 0, 0, 0,
                nullptr, nullptr,
                GetModuleHandleW(nullptr),
                nullptr
        );
    }
    void App::Run(EventCallbacks&& callbacks) {
        callbacks.onReady();
        MSG msg;
        BOOL res;
        while ((res = GetMessageW(&msg, nullptr, 0, 0)) != -1) {
            if (msg.message == WM_MENUCOMMAND) {
                MENUINFO info { };
                info.cbSize = sizeof(info);
                info.fMask = MIM_MENUDATA;
                GetMenuInfo((HMENU)msg.lParam, &info);
                auto clickHandlers = reinterpret_cast<std::vector<std::function<void()>*>*>(info.dwMenuData);
                (*((*clickHandlers)[msg.wParam]))();
            }
            else if (msg.message == WM_QUIT) {
                return;
            }
            else if (msg.hwnd) {
                if (tridentWebViewTranslateMessage != nullptr) {
                    if (tridentWebViewTranslateMessage(&msg)) continue;
                }
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
    }

    void App::Exit(int exitCode) {
        ExitProcess(static_cast<UINT>(exitCode));
    }

    std::string App::GetPath(PathName name) {
        static std::unordered_map<PathName, KNOWNFOLDERID> folderIdByName {
            { PathName::APP_DATA,  FOLDERID_RoamingAppData },
            { PathName::DESKTOP, FOLDERID_Desktop },
            { PathName::DOCUMENTS, FOLDERID_Documents },
            { PathName::DOWNLOADS, FOLDERID_Downloads },
            { PathName::MUSIC, FOLDERID_Music },
            { PathName::PICTURES, FOLDERID_Pictures },
            { PathName::VIDEOS, FOLDERID_Videos },
            { PathName::HOME, FOLDERID_Profile }
        };


        if (name == PathName::TEMP) {
            DWORD length = GetTempPathW(0, NULL);
            std::vector<wchar_t> pathBuffer(length);
            GetTempPathW(length, pathBuffer.data());
            return WStringToUTF8(pathBuffer.data());
        }
        else {
            KNOWNFOLDERID folderId = folderIdByName[name];
            PWSTR pwPath = NULL;
            SHGetKnownFolderPath(folderId, KF_FLAG_CREATE, NULL, &pwPath);
            std::string result = WStringToUTF8(pwPath);
            CoTaskMemFree(pwPath);
            return result;
        }
    }

    std::string App::GetResourcePath(const char* argv0) {
        namespace fs = std::filesystem;
        wchar_t pathBuffer[MAX_PATH];

        DWORD execPathSize = GetModuleFileNameW(nullptr, pathBuffer, MAX_PATH);
        assert(execPathSize > 0);

        std::string execPath = WStringToUTF8(pathBuffer);
        return (fs::path(execPath).parent_path() / "resources").string();
    }

}
