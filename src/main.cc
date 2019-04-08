#include <vector>
#include <cstddef>
#include <thread>
#include <memory>
#include <cassert>
#include <sstream>
#include "platform.h"

namespace node {
    int Start(int argc, char* argv[]);
}

namespace {
    //node::Start requires the strings in argv to be allocated in a continuous memory
    int nodeStart(const std::vector<std::string>& args) {
        std::size_t argv_mem_size = 0;
        for (const std::string& arg: args) {
            argv_mem_size += arg.size() + 1;
        }

        std::vector<char> argv_mem(argv_mem_size);
        std::vector<char*> argv(args.size());

        char* argv_cur = argv_mem.data();
        for (std::size_t i = 0; i < args.size(); i++) {
            const char* c_arg = args[i].c_str();
            std::copy(c_arg, c_arg + args[i].size() + 1, argv_cur);
            argv[i] = argv_cur;
            argv_cur += args[i].size() + 1;
        }

        return node::Start((int)args.size(), argv.data());
    }

    inline void SetEnv(const char* name, const std::string& value) {
        #ifdef WIN32
            _putenv_s(name, value.c_str());
        #else
            setenv(name, value.c_str(), 1);
        #endif
    }
}

//Reference: https://github.com/nodejs/node/blob/v10.15.0/src/node_main.cc
#ifdef WIN32
#include "../core/src/win/util/wstring_utf8.h"
int wmain(int argc, wchar_t* wargv[]) {
    auto nodeArgs = std::vector<std::string>();
    nodeArgs.reserve(argc);
    for (int i = 0; i < argc; ++i) {
        nodeArgs.emplace_back(std::move(WStringToUTF8(wargv[i])));
    }
#else
int main(int argc, char* argv[])
{
    auto nodeArgs = std::vector<std::string>(argv, argv + argc);
#endif

    void* platformData = DeskGapPlatform::InitUIThread();
    std::ostringstream strStream;
    strStream << platformData;
    std::string serializedPlatformData = strStream.str();    

    std::string entry = DeskGapPlatform::PathOfResource({ "app" });
    
    if (const char* env_entry = getenv("DESKGAP_ENTRY"); env_entry != nullptr) {
        if (DeskGapPlatform::ResourceExists({ "app", "DESKGAP_DEFAULT_APP" })) {
            entry = std::string(env_entry);
            SetEnv("NODE_PATH", DeskGapPlatform::PathOfResource({ "node_modules" }));
        }
    }

    std::vector<std::string> insertingArgv = {
        DeskGapPlatform::PathOfResource({ "node_modules", "deskgap" }),
        entry,
        serializedPlatformData
    };
    
    nodeArgs.insert(nodeArgs.begin() + 1, insertingArgv.begin(), insertingArgv.end());

    std::thread nodeThread([nodeArgs = std::move(nodeArgs)] () {
        DeskGapPlatform::InitNodeThread();
        exit(nodeStart(nodeArgs));
    });
    DeskGapPlatform::Run();
	return 0;
}
