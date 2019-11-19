#include <vector>
#include <memory>
#include <thread>
#include <utility>
#include <mutex>
#include <condition_variable>
#include "nod.h"
#include "napi.h"
#include "deskgap/app.hpp"
#include "deskgap/argv.hpp"
#include "node_bindings/index.hpp"
#include "node_bindings/app/app_startup.hpp"
#include "../../lib/src/utils/semaphore.hpp"
extern "C" {
    extern char BIN2CODE_DG_NODE_JS_CONTENT[];
}
namespace {
    Semaphore appRunSemaphore;
    std::vector<std::string> execArgs;
    std::string resourcePath;

    DeskGap::App::EventCallbacks appEventCallbacks;
}

namespace DeskGap {
    const std::vector<std::string>& AppStartup::ExecArgs() {
        return execArgs;
    }
    const std::string& AppStartup::ResourcePath() {
        return resourcePath;
    }
    void AppStartup::SignalAppRun(App::EventCallbacks&& eventCallbacks) {
        appEventCallbacks = std::move(eventCallbacks);
        appRunSemaphore.signal();
    }
}

#ifdef WIN32
int wmain(int argc, const wchar_t** argv)
#else
int main(int argc, const char** argv)
#endif
{
    DeskGap::App::Init();

    std::thread nodeThread([argc, argv]() {
        execArgs = DeskGap::Argv(argc, argv);
        const char* argv0 = execArgs[0].c_str();
        resourcePath = DeskGap::App::GetResourcePath(argv0);
        nod_start(
            argv0,
            BIN2CODE_DG_NODE_JS_CONTENT, true,
            [](napi_env env, napi_value exports) -> napi_value {
                return Napi::RegisterModule(env, exports, DeskGap::InitNodeNativeModule);
            }
        );
    });

    appRunSemaphore.wait();
    DeskGap::App::Run(std::move(appEventCallbacks));

	return 0;
}
