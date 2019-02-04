#include <vector>
#include <cstddef>
#include <thread>
#include <memory>
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
            std::copy_n(args[i].c_str(), args[i].size() + 1, argv_cur);
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

// #ifdef _WIN32
// #include <Windows.h>
 
int main(int argc, char* argv[])
{
    DeskGapPlatform::InitUIThread();

    auto nodeArgs = std::vector<std::string>(argv, argv + argc);

    std::string entry = DeskGapPlatform::PathOfResource({ "app" });
    
    if (DeskGapPlatform::ResourceExists({ "app", "DESKGAP_DEFAULT_APP" })) {
        if (const char* env_entry = getenv("DESKGAP_ENTRY"); env_entry != nullptr) {
            entry = std::string(env_entry);
            SetEnv("NODE_PATH", DeskGapPlatform::PathOfResource({ "node_modules" }));
        }
    };

    std::vector<std::string> insertingArgv = {
        DeskGapPlatform::PathOfResource({ "node_modules", "deskgap" }),
        entry
    };
    
    nodeArgs.insert(nodeArgs.begin() + 1, insertingArgv.begin(), insertingArgv.end());

    std::thread nodeThread([nodeArgs = std::move(nodeArgs)] () {
        DeskGapPlatform::InitNodeThread();
        exit(nodeStart(nodeArgs));
    });

    DeskGapPlatform::Run();
	return 0;
}
