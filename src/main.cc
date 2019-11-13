#include <vector>
#include <memory>
#include <thread>
#include <utility>
#include "nod.h"
#include "deskgap/app.hpp"
#include "deskgap/argv.hpp"

#ifdef WIN32
int wmain(int argc, const wchar_t** argv)
#else
int main(int argc, const char** argv)
#endif
{
    std::vector<std::string> args = DeskGap::Argv(argc, argv);
    std::thread node_thread([ args { std::move(args) }]() {
        nod_start(args[0].c_str(), "console.log(process.execPath);setInterval(() => console.log(12131), 500)", true, nullptr);
    });
    DeskGap::App::Run({
        []() {
            printf("on ready\n");
        },
        []() {
            printf("before quit\n");
            DeskGap::App::Exit(0);
        }
    });

//    void* platformData = DeskGapPlatform::InitUIThread();
//    std::ostringstream strStream;
//    strStream << platformData;
//    std::string serializedPlatformData = strStream.str();
//
//    std::string entry = DeskGapPlatform::PathOfResource({ "app" });
//
//    if (const char* env_entry = getenv("DESKGAP_ENTRY"); env_entry != nullptr) {
//        if (DeskGapPlatform::ResourceExists({ "app", "DESKGAP_DEFAULT_APP" })) {
//            entry = std::string(env_entry);
//            SetEnv("NODE_PATH", DeskGapPlatform::PathOfResource({ "node_modules" }));
//        }
//    }
//
//    std::vector<std::string> insertingArgv = {
//        DeskGapPlatform::PathOfResource({ "node_modules", "deskgap" }),
//        entry,
//        serializedPlatformData
//    };
//
//    nodeArgs.insert(nodeArgs.begin() + 1, insertingArgv.begin(), insertingArgv.end());
//
//    std::thread nodeThread([nodeArgs = std::move(nodeArgs)] () {
//        DeskGapPlatform::InitNodeThread();
//        exit(nodeStart(nodeArgs));
//    });
//    DeskGapPlatform::Run();
	return 0;
}
