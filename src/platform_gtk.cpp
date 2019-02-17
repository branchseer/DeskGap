#include "platform.h"
#include <filesystem>

namespace fs = std::filesystem;

void DeskGapPlatform::InitUIThread() {

}

void DeskGapPlatform::InitNodeThread() { }

void DeskGapPlatform::Run() {
    
}

std::string DeskGapPlatform::PathOfResource(const std::vector<const char*>& paths) {
    static fs::path resourcesFolder = fs::read_symlink("/proc/self/exe").parent_path() / "resources";

    fs::path resourcePath = resourcesFolder;
    for (const char* component: paths) {
        resourcePath.append(component);
    }

    printf("%s\n", resourcePath.c_str());
    return resourcePath;
}

bool DeskGapPlatform::ResourceExists(const std::vector<const char*>& paths) {
    return fs::exists(PathOfResource(paths));
}
