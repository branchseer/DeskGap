#ifndef ui_dispatch_h
#define ui_dispatch_h

#include <functional>
#include <string>
#include <node_api.h>

namespace DeskGap {
    void UISync(napi_env env, std::function<void()>&& action);

    void DelayUISync();
    void CommitUISync(napi_env env);
    
    void UISyncDelayable(napi_env env, std::function<void()>&& action);
    void UIASync(napi_env env, std::function<void()>&& action);
}

#endif /* ui_dispatch_h */
