#ifndef HOST_H
#define HOST_H

#include <string>
#include <vector>

#if _WIN32
    #define HOST_EXPORT __declspec(dllexport)
#else
    #define HOST_EXPORT
#endif

namespace DeskGapPlatform {
    HOST_EXPORT void InitUIThread();
    HOST_EXPORT void InitNodeThread();
    HOST_EXPORT void Run();

    HOST_EXPORT std::string PathOfResource(const std::vector<const char*>& name);
    HOST_EXPORT bool ResourceExists(const std::vector<const char*>& name);
}

#endif
