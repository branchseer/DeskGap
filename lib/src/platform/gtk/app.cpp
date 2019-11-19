#include <gtk/gtk.h>
#include <functional>
#include <memory>
#include <cstdlib>
#include <unordered_map>
#include <filesystem>

#include "app.hpp"
#include "util/xdg-user-dir-lookup.h"

using std::shared_ptr;
using std::function;
using std::make_shared;

namespace DeskGap {
    void App::Run(EventCallbacks&& callbacks) {
        GtkApplication* gtkApp = gtk_application_new(nullptr, G_APPLICATION_FLAGS_NONE);
        g_application_hold(G_APPLICATION(gtkApp));
        // Suppress no activate handler warning:
        g_signal_connect(gtkApp, "activate", G_CALLBACK([](){ }), nullptr);
        callbacks.onReady();
        g_application_run(G_APPLICATION(gtkApp), 0, NULL);
        g_object_unref(gtkApp);
    }
    
    void App::Exit(int exitCode) {
        std::exit(exitCode);
    }

    std::string App::GetPath(PathName name) {
        static std::unordered_map<PathName, const char*> xdgDirTypeByPathName {
            { PathName::DESKTOP, "DESKTOP" },
            { PathName::DOCUMENTS, "DOCUMENTS" },
            { PathName::DOWNLOADS, "DOWNLOAD" },
            { PathName::MUSIC, "MUSIC" },
            { PathName::PICTURES, "PICTURES" },
            { PathName::VIDEOS, "VIDEOS" },
        };

        if (name == PathName::APP_DATA) {
            if (const char* xdgConfigHome = getenv("XDG_CONFIG_HOME"); xdgConfigHome != nullptr) {
                return xdgConfigHome;
            }
            else {
                return std::string(g_get_home_dir()) + "/.config";
            }
            return g_get_home_dir();
        }
        else if (name == PathName::HOME) {
            return g_get_home_dir();
        }
        else if (name == PathName::TEMP) {
            return g_get_tmp_dir();
        }
        else {
            char* cresult = xdg_user_dir_lookup(xdgDirTypeByPathName[name]);
            std::string result(cresult);
            free(cresult);
            return std::move(result);
        }
    }

    std::string App::GetResourcePath(const char* argv0) {
        namespace fs = std::filesystem;
        std::error_code err;
        fs::path execPath = fs::read_symlink("/proc/self/exe", err);
        if (err) {
            execPath.assign(argv0);
        }
        return (execPath.parent_path() / "resources").string();
    }
}
