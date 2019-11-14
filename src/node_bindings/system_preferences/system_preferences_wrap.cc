#include <deskgap/system_preferences.hpp>
#include "system_preferences_wrap.h"
#include "../dispatch/node_dispatch.h"

Napi::Object DeskGap::SystemPreferencesObject(const Napi::Env& env) {
    Napi::Object object = Napi::Object::New(env);
    #ifdef __APPLE__
    object.Set("getUserDefaultInteger", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        long value = SystemPreferences::GetUserDefaultInteger(info[0].As<Napi::String>());
        return Napi::Number::New(info.Env(), value);
    }));
    object.Set("getUserDefaultFloat", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        float value = SystemPreferences::GetUserDefaultFloat(info[0].As<Napi::String>());
        return Napi::Number::New(info.Env(), value);
    }));
    object.Set("getUserDefaultDouble", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        double value = SystemPreferences::GetUserDefaultDouble(info[0].As<Napi::String>());
        return Napi::Number::New(info.Env(), value);
    }));
    object.Set("getUserDefaultString", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        std::string value = SystemPreferences::GetUserDefaultString(info[0].As<Napi::String>());
        return Napi::String::New(info.Env(), value);
    }));
    object.Set("getUserDefaultURL", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        std::string value = SystemPreferences::GetUserDefaultURL(info[0].As<Napi::String>());
        return Napi::String::New(info.Env(), value);
    }));
    object.Set("getUserDefaultBool", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        bool value = SystemPreferences::GetUserDefaultBool(info[0].As<Napi::String>());
        return Napi::Boolean::New(info.Env(), value);
    }));
    object.Set("getUserDefaultArrayJSON", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        std::string value = SystemPreferences::GetUserDefaultArrayJSON(info[0].As<Napi::String>());
        return Napi::String::New(info.Env(), value);
    }));
    object.Set("getUserDefaultDictionaryJSON", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        std::string value = SystemPreferences::GetUserDefaultDictionaryJSON(info[0].As<Napi::String>());
        return Napi::String::New(info.Env(), value);
    }));
    #endif

    object.Set("getAndWatchDarkMode", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        auto jsCallback = JSFunctionForUI::Persist(info[0].As<Napi::Function>());
        bool isDarkMode = SystemPreferences::GetAndWatchDarkMode([
            jsCallback = std::move(jsCallback)
        ]() {
            jsCallback->Call();
        });
        return Napi::Boolean::New(info.Env(), isDarkMode);
    }));

    return object;
}
