#include <cstdlib>
#include <shlobj_core.h>
#include <fileapi.h>

#include "../app/app.h"

using namespace System;

namespace {
    std::string UTF8FromWideChars(const wchar_t* wideChars, size_t wideCharCount) {
        if (wideCharCount == 0) return std::string();
		size_t resultSize = WideCharToMultiByte(
			CP_UTF8, 0, wideChars, wideCharCount,
			NULL, 0, NULL, NULL
		);
		std::string result(resultSize, 0);
		WideCharToMultiByte(
			CP_UTF8, 0, wideChars, wideCharCount,
			result.data(), resultSize, NULL, NULL
		);
		return result;
    }
}

namespace DeskGap {
    struct App::Impl {
        App::EventCallbacks callbacks_;
    };

    App::App(EventCallbacks&& callbacks): impl_(std::make_unique<Impl>()) {
        impl_->callbacks_ = std::move(callbacks);
    }
    void App::Run() {
        impl_->callbacks_.onReady();
    }
    
    void App::Exit(int exitCode) {
        Environment::Exit(exitCode);
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
            return UTF8FromWideChars(pathBuffer.data(), length);
        }
        else {
            KNOWNFOLDERID folderId = folderIdByName[name];
            PWSTR pwPath = NULL;
            SHGetKnownFolderPath(folderId, KF_FLAG_CREATE, NULL, &pwPath);
            std::string result = UTF8FromWideChars(pwPath, wcslen(pwPath));
            CoTaskMemFree(pwPath);
            return result;
        }
    }

    App::~App() = default;
}
