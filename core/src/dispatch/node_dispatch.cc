#include <memory>
#include <napi.h>
#include <uv.h>
#include <cassert>
#include "node_dispatch.h"

// using action_t = std::function<void()>;

//https://github.com/nodejs/node/issues/13512#issuecomment-306966848
// void DeskGap::NodeAsync(const action_t& action) {
//     struct async_data_t {
//         action_t action;
//     };
//     static uv_loop_t* uv_loop = nullptr;
//     if (uv_loop == nullptr) {
//         napi_get_uv_event_loop(GetNodeEnv(), &uv_loop);
//     }

//     uv_async_t* async = new uv_async_t;
    
//     uv_async_init(uv_loop, async, [](uv_async_t* async) {
//         auto async_data = (async_data_t*)uv_handle_get_data((uv_handle_t*)async);
//         {
//             Napi::HandleScope scope(GetNodeEnv());
//             try {
//                 (async_data->action)();
//             } catch (const Napi::Error& e) {
//                 napi_fatal_exception(e.Env(), e.Value());
//             }
//         }

//         uv_close((uv_handle_t*)async, [](uv_handle_t* async) {
//             auto async_data = (async_data_t*)uv_handle_get_data((uv_handle_t*)async);
//             delete (uv_async_t*)async;
//             delete async_data;
//         });
//     });

//     uv_handle_set_data((uv_handle_t*)async, new async_data_t {
//         action
//     });

//     uv_async_send(async);
// }

// void DeskGap::NodeAsync(const std::function<void(const Napi::Env&)>& action) {
//     DeskGap::NodeAsync(static_cast<action_t>(std::bind(action, GetNodeEnv())));
// }


namespace DeskGap {
    JSFunctionForUI::JSFunctionForUI(const Napi::Function& js_func) {
        napi_status status = napi_create_threadsafe_function(
            js_func.Env(), js_func,
            /*async_resource*/nullptr, /*async_resource_name*/Napi::String::New(js_func.Env(), "JSFunctionForUI"),
            /*max_queue_size*/0, /*initial_thread_count*/1, 
            /*thread_finalize_data*/nullptr, /*thread_finalize_cb*/nullptr,
            /*context*/this, /*call_js_cb*/JSFunctionForUI::call_js_cb,
            &threadsafe_function_
        );
        assert(status == napi_ok);
    }

    void JSFunctionForUI::Call_(std::optional<JSArgsGetter>&& getArgs) {
        napi_status status = napi_call_threadsafe_function(
            threadsafe_function_,
            getArgs.has_value() ? new JSArgsGetter(std::move(*getArgs)): nullptr,
            napi_tsfn_blocking
        );
        assert(status == napi_ok);
    }

    void JSFunctionForUI::Call() {
        Call_(std::nullopt);
    }
    void JSFunctionForUI::Call(JSArgsGetter&& getter) {
        Call_(std::make_optional<JSArgsGetter>(std::move(getter)));
    }

    void JSFunctionForUI::call_js_cb(napi_env env, napi_value js_callback, void* context, void* data) {
        auto jsArgsGetter = static_cast<JSArgsGetter*>(data);
        if (env != nullptr && js_callback != nullptr) {
            Napi::Function func = Napi::Function(env, js_callback);
            try {
                func.Call(jsArgsGetter != nullptr ? (*jsArgsGetter)(env) : std::vector<napi_value>());
            }
            catch (const Napi::Error& e) {
                napi_fatal_exception(e.Env(), e.Value());
            }
        }
        if (jsArgsGetter != nullptr) {
            delete jsArgsGetter;
        }
    }
    JSFunctionForUI::~JSFunctionForUI() {
        napi_status status = napi_release_threadsafe_function(threadsafe_function_, napi_tsfn_abort);
        assert(status == napi_ok);
    }

    std::shared_ptr<JSFunctionForUI> JSFunctionForUI::Persist(const Napi::Function& func) {
        return std::make_shared<JSFunctionForUI>(func);
    }

    std::shared_ptr<JSFunctionForUI> JSFunctionForUI::Weak(const Napi::Function& func) {
        return Persist(Napi::Function::New(func.Env(), [
            weakFunc = std::make_shared<Napi::FunctionReference>(Napi::Weak(func))
        ](const Napi::CallbackInfo& info) {
            if (weakFunc->IsEmpty()) return;
            std::vector<napi_value> args;
            size_t argsCount = info.Length();
            args.reserve(argsCount);
            for (size_t i = 0; i < argsCount; ++i) {
                args.push_back(info[i]);
            }
            weakFunc->Call(args);
        }));
    }
}
