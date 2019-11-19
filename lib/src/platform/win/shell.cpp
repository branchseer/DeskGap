#include "shell.hpp"
#include "./util/wstring_utf8.h"
#include <Windows.h>
#include <shellapi.h>

bool DeskGap::Shell::OpenExternal(const std::string& urlString) {
	std::wstring wUrlString = UTF8ToWString(urlString.c_str());
    return ShellExecuteW(
    	nullptr, L"open",
    	wUrlString.c_str(),
    	nullptr, nullptr,
    	SW_SHOWNORMAL
    ) > (HINSTANCE)32;
}
