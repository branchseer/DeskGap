#ifndef PLATFORM_H
#define PLATFORM_H

#include <string>
#include <vector>

#if _WIN32
    #define PLATFORM_EXPORT __declspec(dllexport)
#else
    #define PLATFORM_EXPORT
#endif

namespace DeskGapPlatform {
    HOST_EXPORT void InitUIThread();
    HOST_EXPORT void InitNodeThread();
    HOST_EXPORT void Run();

    HOST_EXPORT std::string PathOfResource(const std::vector<const char*>& name);
    HOST_EXPORT bool ResourceExists(const std::vector<const char*>& name);
}

#endif
