#include <filesystem>
#include <memory>
#include <gtkmm.h>

#include "platform.h"

namespace fs = std::filesystem;

namespace {
    Glib::RefPtr<Gtk::Application> app;
}

void DeskGapPlatform::InitUIThread() {
    app = Gtk::Application::create();
    app->hold();
}

void DeskGapPlatform::InitNodeThread() { }

void DeskGapPlatform::Run() {
    app->run();
    app.reset();
}

std::string DeskGapPlatform::PathOfResource(const std::vector<const char*>& paths) {
    static fs::path resourcesFolder = fs::read_symlink("/proc/self/exe").parent_path() / "resources";

    fs::path resourcePath = resourcesFolder;
    for (const char* component: paths) {
        resourcePath.append(component);
    }

    return resourcePath;
}

bool DeskGapPlatform::ResourceExists(const std::vector<const char*>& paths) {
    return fs::exists(PathOfResource(paths));
}
