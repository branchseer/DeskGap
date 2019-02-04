#include "shell_wrap.h"
#include "shell.h"

Napi::Object DeskGap::ShellObject(const Napi::Env& env) {
    Napi::Object shellObject = Napi::Object::New(env);
    shellObject.Set("openExternal",  Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        bool success = Shell::OpenExternal(info[0].As<Napi::String>());
        return Napi::Boolean::New(info.Env(), success);
    }));

    return shellObject;
}
