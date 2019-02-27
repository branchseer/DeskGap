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
    PLATFORM_EXPORT void* InitUIThread();
    PLATFORM_EXPORT void InitNodeThread();
    PLATFORM_EXPORT void Run();

    PLATFORM_EXPORT std::string PathOfResource(const std::vector<const char*>& name);
    PLATFORM_EXPORT bool ResourceExists(const std::vector<const char*>& name);
}

#endif
