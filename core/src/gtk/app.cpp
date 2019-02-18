#include <gtk/gtk.h>
#include <functional>
#include <memory>
#include <cstdlib>
#include <unordered_map>

#include "../app/app.h"

using std::shared_ptr;
using std::function;
using std::make_shared;

namespace DeskGap {
    struct App::Impl {
    	App::EventCallbacks callbacks;
    };

    App::App(EventCallbacks&& callbacks): impl_(std::make_unique<Impl>()) {
    	impl_->callbacks = std::move(callbacks);
    }
    
    void App::Run() {
    	impl_->callbacks.onReady();
    }
    
    void App::Exit(int exitCode) {
        std::exit(exitCode);
    }

    std::string App::GetPath(PathName name) {
        return "path";
    }
    
    App::~App() = default;
}

