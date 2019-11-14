#include <optional>
#include "dialog_wrap.h"
#include <deskgap/dialog.hpp>
#include "../dispatch/ui_dispatch.h"
#include "../dispatch/node_dispatch.h"
#include "../util/js_native_convert.h"
#include "../window/browser_window_wrap.h"

namespace DeskGap::JSNativeConvertion {
    using CommonFileDialogOptions = DeskGap::Dialog::CommonFileDialogOptions;
    using OpenDialogOptions = DeskGap::Dialog::OpenDialogOptions;
    using SaveDialogOptions = DeskGap::Dialog::SaveDialogOptions;
    using FileFilter = CommonFileDialogOptions::FileFilter;

    template<>
    struct Native<FileFilter> {
        inline static FileFilter From(const Napi::Value& jsValue) {
            Napi::Object jsObject = jsValue.As<Napi::Object>();
            FileFilter result;
            ToNative(result.name, jsObject.Get("name"));
            ToNative(result.extensions, jsObject.Get("extensions"));
            return result;
        }
    };

    template<>
    struct Native<CommonFileDialogOptions> {
        inline static CommonFileDialogOptions From(const Napi::Value& jsValue) {
            Napi::Object jsOptions = jsValue.As<Napi::Object>();
            CommonFileDialogOptions options;

            ToNative(options.title, jsOptions.Get("title"));
            ToNative(options.defaultDirectory, jsOptions.Get("defaultDirectory"));
            ToNative(options.defaultFilename, jsOptions.Get("defaultFilename"));
            ToNative(options.buttonLabel, jsOptions.Get("buttonLabel"));
            ToNative(options.filters, jsOptions.Get("filters"));
            ToNative(options.message, jsOptions.Get("message"));

            return options;
        }
    };

    template<>
    struct Native<OpenDialogOptions> {
        inline static OpenDialogOptions From(const Napi::Value& jsValue) {
            Napi::Object jsOptions = jsValue.As<Napi::Object>();
            
            OpenDialogOptions options;
            ToNative(options.commonOptions, jsOptions.Get("commonOptions"));
            ToNative(options.properties, jsOptions.Get("propertyBits"));

            return options;
        }
    };

    template<>
    struct Native<SaveDialogOptions> {
        inline static SaveDialogOptions From(const Napi::Value& jsValue) {
            Napi::Object jsOptions = jsValue.As<Napi::Object>();
            
            SaveDialogOptions options;
            ToNative(options.commonOptions, jsOptions.Get("commonOptions"));
            ToNative(options.nameFieldLabel, jsOptions.Get("nameFieldLabel"));
            ToNative(options.showsTagField, jsOptions.Get("showsTagField"));

            return options;
        }
    };
}

Napi::Object DeskGap::DialogObject(const Napi::Env& env) {
    using namespace DeskGap::JSNativeConvertion;

    Napi::Object dialogObject = Napi::Object::New(env);
    dialogObject.Set("showErrorBox",  Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        UISyncDelayable(info.Env(), [
            title = Native<std::string>::From(info[0]),
            content = Native<std::string>::From(info[1])
        ]() {
            Dialog::ShowErrorBox(title, content);
        });
    }));
    

    dialogObject.Set("showOpenDialog", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        auto browserWindow = OptionalUnderlying<BrowserWindowWrap>(info[0]);
        auto options = Native<Dialog::OpenDialogOptions>::From(info[1]);
        auto callback = JSFunctionForUI::Persist(info[2].As<Napi::Function>());

        UIASync(info.Env(), [
            browserWindow = std::move(browserWindow),
            callback = std::move(callback),
            options = std::move(options)
        ]() {
            Dialog::ShowOpenDialog(browserWindow, options, [callback = std::move(callback)](Dialog::OpenDialogResult&& result) {
                callback->Call([result = std::move(result), callback](auto env) {
                    return std::vector<napi_value> {
                        JSFrom(env, result.filePaths)
                    };
                });
            });
        });
    }));

    dialogObject.Set("showSaveDialog", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
        auto browserWindow = OptionalUnderlying<BrowserWindowWrap>(info[0]);
        auto options = Native<Dialog::SaveDialogOptions>::From(info[1]);
        auto callback = JSFunctionForUI::Persist(info[2].As<Napi::Function>());

        UIASync(info.Env(), [
            browserWindow = std::move(browserWindow),
            callback = std::move(callback),
            options = std::move(options)
        ]() {
            Dialog::ShowSaveDialog(browserWindow, options, [callback = std::move(callback)](Dialog::SaveDialogResult&& result) {
                callback->Call([result = std::move(result), callback](auto env) {
                    return std::vector<napi_value> {
                        JSFrom(env, result.filePath)
                    };
                });
            });
        });

    }));

    return dialogObject;
}
