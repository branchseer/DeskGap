#include <filesystem>
#include <memory>
#include <gtk/gtk.h>

#include "platform.h"

namespace fs = std::filesystem;

namespace {
    void deleteGObject(gpointer object) {
        if (object != nullptr) {
            g_object_unref(object);
        }
    }

    std::unique_ptr<GtkApplication, decltype(&deleteGObject)> gtkApp(nullptr, deleteGObject);
}

void DeskGapPlatform::InitUIThread() {
    gtkApp.reset(gtk_application_new(nullptr, G_APPLICATION_FLAGS_NONE));
    g_application_hold(G_APPLICATION(gtkApp.get()));
    
    // Suppress no activate handler warning:
    g_signal_connect(gtkApp.get(), "activate", G_CALLBACK([](){ }), NULL);

}

void DeskGapPlatform::InitNodeThread() { }

void DeskGapPlatform::Run() {
    g_application_run(G_APPLICATION(gtkApp.get()), 0, NULL);
    gtkApp.reset(nullptr);
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
