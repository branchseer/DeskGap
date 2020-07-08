#include <deskgap/shell.hpp>
#include "shell_wrap.h"
#include "../dispatch/ui_dispatch.h"

Napi::Object DeskGap::ShellObject(const Napi::Env& env) {
    Napi::Object shellObject = Napi::Object::New(env);
    shellObject.Set("openExternal",  Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        bool success;
        std::string urlString = info[0].As<Napi::String>();
        UISync(info.Env(), [&]() {
        	success = Shell::OpenExternal(urlString);
        });
        return Napi::Boolean::New(info.Env(), success);
    }));
    shellObject.Set("showItemInFolder", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        std::string pathString = info[0].As<Napi::String>();
        UISync(info.Env(), [&]() {
        	Shell::ShowItemInFolder(pathString);
        });
    }));
    return shellObject;
}
