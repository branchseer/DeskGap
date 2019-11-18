#include <memory>
#include <napi.h>
#include <cassert>
#include "node_dispatch.h"

namespace DeskGap {
    namespace {
        struct ThreadSafeFunctionData {
            std::optional<JSFunctionForUI::JSArgsGetter> jsArgsGetter;
            napi_threadsafe_function holdedThreadSafeFunction;
        };
    }
    JSFunctionForUI::JSFunctionForUI(const Napi::Function& js_func, bool holdWhileQueuing): holdWhileQueuing_(holdWhileQueuing) {
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
        napi_status status;
        auto data = new ThreadSafeFunctionData { std::move(getArgs), nullptr };
        if (holdWhileQueuing_) {
            status = napi_acquire_threadsafe_function(threadsafe_function_);
            assert(status == napi_ok);

            data->holdedThreadSafeFunction = threadsafe_function_;
        }
        status = napi_call_threadsafe_function(
            threadsafe_function_,
            data,
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

    void JSFunctionForUI::call_js_cb(napi_env env, napi_value js_callback, void* context, void* untypedData) {
        auto data = static_cast<ThreadSafeFunctionData*>(untypedData);
        if (env != nullptr && js_callback != nullptr) {
            Napi::Function func = Napi::Function(env, js_callback);
            try {
                func.Call(data->jsArgsGetter.has_value() ? (*(data->jsArgsGetter))(env) : std::vector<napi_value>());
            }
            catch (const Napi::Error& e) {
                napi_fatal_exception(e.Env(), e.Value());
            }
            if (data->holdedThreadSafeFunction != nullptr) {
                napi_status status = napi_release_threadsafe_function(data->holdedThreadSafeFunction, napi_tsfn_release);
                assert(status == napi_ok);
            }
        }

        delete data;
    }
    JSFunctionForUI::~JSFunctionForUI() {
        napi_status status = napi_release_threadsafe_function(threadsafe_function_, napi_tsfn_release);
        assert(status == napi_ok);
    }

    std::shared_ptr<JSFunctionForUI> JSFunctionForUI::Persist(const Napi::Function& func, bool holdWhileQueuing) {
        return std::make_shared<JSFunctionForUI>(func, holdWhileQueuing);
    }
}
