#ifndef PLATFORM_H
#define PLATFORM_H

#include <string>
#include <vector>

namespace DeskGapPlatform {
    void* InitUIThread();
    void InitNodeThread();
    void Run();

    std::string PathOfResource(const std::vector<const char*>& name);
    bool ResourceExists(const std::vector<const char*>& name);
}

#endif
