#ifndef node_dispatch_h
#define node_dispatch_h

#include <memory>
#include <functional>
#include <optional>
#include <napi.h>
#include <vector>
#include <node_api.h>

namespace DeskGap {

    class JSFunctionForUI {
    public:
        JSFunctionForUI(const JSFunctionForUI&) = delete;
        JSFunctionForUI(const Napi::Function& js_func, bool holdWhileQueuing);
        JSFunctionForUI& operator=(const JSFunctionForUI&) = delete;

        static std::shared_ptr<JSFunctionForUI> Persist(const Napi::Function&, bool holdWhileQueuing = false);
        static std::shared_ptr<JSFunctionForUI> Weak(const Napi::Function&);

        ~JSFunctionForUI();

        using JSArgsGetter = std::function<std::vector<napi_value>(napi_env)>;

        void Call(JSArgsGetter&&);
        void Call();
    private:

        bool holdWhileQueuing_;
        napi_threadsafe_function threadsafe_function_;
        static void call_js_cb(napi_env env, napi_value js_callback, void* context, void* data);
        void Call_(std::optional<JSArgsGetter>&& getArgs);
    };
}

#endif /* node_dispatch_h */
