#include <vector>
#include <utility>
#include <functional>
#include "../window/browser_window_wrap.h"

namespace DeskGap::JSNativeConvertion {

    template<class Wrap>
    using UnderlyingObjectOf = decltype(std::declval<Wrap>().UnderlyingObject());

    template<class Wrap>
    inline UnderlyingObjectOf<Wrap> Underlying(const Napi::Value& jsWrapperObject) {
        auto* wrapObject = Wrap::Unwrap(jsWrapperObject.As<Napi::Object>());
        return wrapObject->UnderlyingObject();
    }

    template<class Wrap>
    inline std::optional<UnderlyingObjectOf<Wrap>> OptionalUnderlying(const Napi::Value& jsWrapperObject) {
        if (jsWrapperObject.IsNull()) return std::nullopt;
        return std::make_optional(Underlying<Wrap>(jsWrapperObject));
    }


    template<class T>
    struct Native {
        inline static T From(const Napi::Value&);
    };

    template<>
    struct Native<std::string> {
        inline static std::string From(const Napi::Value& jsValue) {
            return jsValue.As<Napi::String>().Utf8Value();
        }
    };
    template<>
    struct Native<double> {
        inline static double From(const Napi::Value& jsValue) {
            return jsValue.As<Napi::Number>().DoubleValue();
        }
    };
    template<>
    struct Native<uint32_t> {
        inline static uint32_t From(const Napi::Value& jsValue) {
            return jsValue.As<Napi::Number>().Uint32Value();
        }
    };
    template<>
    struct Native<bool> {
        inline static bool From(const Napi::Value& jsValue) {
            return jsValue.As<Napi::Boolean>().Value();
        }
    };

    template<class E>
    struct Native<std::optional<E>> {
        inline static std::optional<E> From(const Napi::Value& jsValue) {
            return jsValue.IsNull() ? std::nullopt: std::make_optional<E>(Native<E>::From(jsValue));
        }
    };

    template<class E>
    struct Native<std::vector<E>> {
        inline static std::vector<E> From(const Napi::Value& jsValue) {
            Napi::Array jsArray = jsValue.As<Napi::Array>();
            size_t arrayLength = jsArray.Length();
            std::vector<E> result;
            result.reserve(arrayLength);
            for (size_t i = 0; i < arrayLength; i++) {
                result.push_back(Native<E>::From(jsArray.Get(i)));
            }
            return std::move(result);
        }
    };

    template<class T>
    void ToNative(T& nativeValue, const Napi::Value& jsValue) {
        nativeValue = Native<T>::From(jsValue);
    }

    template<class T>
    struct JS {
        inline static Napi::Value From(napi_env, const T&);
    };

    template<>
    struct JS<std::string> {
        inline static Napi::Value From(napi_env env,const std::string& utf8string) {
            return Napi::String::New(env, utf8string);
        }
    };
    template<class E>
    struct JS<std::optional<E>> {
        inline static Napi::Value From(napi_env env, const std::optional<E>& optionalValue) {
            return optionalValue.has_value() ? JS<E>::From(env, *optionalValue): Napi::Env(env).Null();
        }
    };
    template<class E>
    struct JS<std::vector<E>> {
        inline static Napi::Value From(napi_env env, const std::vector<E>& values) {
            Napi::Array array = Napi::Array::New(env, values.size());
            for (size_t i = 0; i < values.size(); ++i) {
                array.Set(i, JS<E>::From(env, values[i]));
            }
            return array;
        }
    };

    template<class T>
    inline Napi::Value JSFrom(napi_env env, T val) {
        return JS<T>::From(env, val);
    }
}
