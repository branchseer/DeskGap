#ifndef node_dispatch_h
#define node_dispatch_h

#include <memory>
#include <functional>
#include <optional>
#include <napi.h>
#include <vector>
#include <node_api.h>

namespace DeskGap {
    // void NodeAsync(const std::function<void()>& action);
    // void NodeAsync(const std::function<void(const Napi::Env&)>& action);

    class JSFunctionForUI {
    public:
        JSFunctionForUI(const JSFunctionForUI&) = delete;
        JSFunctionForUI(const Napi::Function& js_func);
        JSFunctionForUI& operator=(const JSFunctionForUI&) = delete;

        static std::shared_ptr<JSFunctionForUI> Persist(const Napi::Function&);
        static std::shared_ptr<JSFunctionForUI> Weak(const Napi::Function&);

        ~JSFunctionForUI();

        using JSArgsGetter = std::function<std::vector<napi_value>(napi_env)>;

        void Call(JSArgsGetter&&);
        void Call();
    private:
        napi_threadsafe_function threadsafe_function_;
        static void call_js_cb(napi_env env, napi_value js_callback, void* context, void* data);
        void Call_(std::optional<JSArgsGetter>&& getArgs);
    };

    // inline std::shared_ptr<JSFunctionForUI> JSFunctionForUI::Weak(const Napi::Function& js_func) {
    //     return std::make_shared<JSFunctionForUI>(js_func);
    // }


    // template<typename Args...>
    // std::function<void(Args...)> UIFunction()
}

#endif /* node_dispatch_h */
