#include <Windows.h>
#include <Ole2.h>
#include <Combaseapi.h>
#include <Shellscalingapi.h>
#include <utility>
#include <filesystem>
#include <functional>
#include <vector>
#include <cassert>

#include "../../../../src/platform.h"

#include "../../../../core/src/win/util/wstring_utf8.h"
#include "../../../../core/src/win/platform_data.h"

namespace fs = std::filesystem;

namespace {
    std::string GetExecutablePath() {
        wchar_t path[MAX_PATH];
        DWORD result = GetModuleFileNameW(nullptr, path, MAX_PATH);
        assert(result > 0);
        return WStringToUTF8(path);
    }
    const wchar_t* const DispatcherWndClassName = L"DeskGapDispatcherWindow";
    DeskGap::PlatformData* platformData;
}



void* DeskGapPlatform::InitUIThread() {

    platformData = new DeskGap::PlatformData { dispatchWindowWnd, nullptr };
    return platformData;
}

void DeskGapPlatform::InitNodeThread() {
    
}

void DeskGapPlatform::Run() {
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
            if (platformData->tridentWebViewTranslateMessage != nullptr) {
                if (platformData->tridentWebViewTranslateMessage(&msg)) continue;
            }
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}
}

std::string DeskGapPlatform::PathOfResource(const std::vector<const char*>& paths) {
    static fs::path resourcesFolder = fs::path(GetExecutablePath()).parent_path() / "resources";
    
    fs::path resourcePath = resourcesFolder;
    for (const char* component: paths) {
        resourcePath.append(component);
    }

    return resourcePath.string();
}

bool DeskGapPlatform::ResourceExists(const std::vector<const char*>& paths) {
    return fs::exists(PathOfResource(paths));
}
