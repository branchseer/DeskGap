#include <Windows.h>
#include <Commctrl.h>
#include <Combaseapi.h>
#include <Shellscalingapi.h>
#include <vcclr.h>
#include <utility>

#include "platform.h"

using namespace System;
using namespace System::Threading;
using namespace System::Windows::Forms;

namespace {
    gcroot<SynchronizationContext^> syncContext;

    inline String^ ClrStr(const char* utf8string) {
        return gcnew System::String(
            utf8string, 0, strlen(utf8string), System::Text::Encoding::UTF8
        );
    }

    inline std::string StdStr(System::String^ clrString) {
        array<unsigned char>^ bytes = System::Text::Encoding::UTF8->GetBytes(clrString);
        if (bytes->Length == 0) {
            return { };
        }
        else {
            pin_ptr<System::Byte> pinned = &bytes[0];
            unsigned char* cstr = pinned;
            return reinterpret_cast<char*>(cstr);
        }
    }

    String^ pathOfResource(const std::vector<const char*>& paths) {
        array<String^>^ allPaths = gcnew array<String^>(paths.size() + 2);
        allPaths[0] = Application::StartupPath;
        allPaths[1] = "resources";
        for (size_t i = 0; i < paths.size(); ++i) {
            allPaths[i + 2] = ClrStr(paths[i]);
        }
        return System::IO::Path::Combine(allPaths);
    }
}

void DeskGapPlatform::InitUIThread() { 
    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);

    //A control need to be constructed for SynchronizationContext to be ready for the UI thread.
    gcnew Control();

    syncContext = SynchronizationContext::Current;
}

void DeskGapPlatform::InitNodeThread() {
    //Attach the SynchronizationContext to the node thread.
    //So the nodejs native addon will be able to obtain it and use it to dispatch actions to the UI thread.
    SynchronizationContext::SetSynchronizationContext(syncContext);
    syncContext = nullptr;
}

void DeskGapPlatform::Run() {
    Application::Run();
}

std::string DeskGapPlatform::PathOfResource(const std::vector<const char*>& paths) {
    return StdStr(System::IO::Path::Combine(pathOfResource(paths)));
}

bool DeskGapPlatform::ResourceExists(const std::vector<const char*>& paths) {
    return System::IO::File::Exists(pathOfResource(paths));
}
