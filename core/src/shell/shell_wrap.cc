#include "shell_wrap.h"
#include "shell.h"
#include "../dispatch/ui_dispatch.h"

Napi::Object DeskGap::ShellObject(const Napi::Env& env) {
    Napi::Object shellObject = Napi::Object::New(env);
    shellObject.Set("openExternal",  Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        bool success;
        UISync(info.Env(), [&success, urlString = info[0].As<Napi::String>()]() {
        	success = Shell::OpenExternal(urlString);
        });
        return Napi::Boolean::New(info.Env(), success);
    }));

    return shellObject;
}
