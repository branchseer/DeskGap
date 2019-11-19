#ifndef gtk_util_convert_js_result_h
#define gtk_util_convert_js_result_h

#include <string>
#include <optional>
#include <webkit2/webkit2.h>
#include <JavaScriptCore/JSValueRef.h>
#include <JavaScriptCore/JSStringRef.h>

namespace {
	std::optional<std::string> jsResultToString(WebKitJavascriptResult* jsResult) {
		JSGlobalContextRef context = webkit_javascript_result_get_global_context (jsResult);
    	JSValueRef value = webkit_javascript_result_get_value (jsResult);

    	if (!JSValueIsString(context, value)) {
    		return std::nullopt;
    	}

        JSStringRef jsString = JSValueToStringCopy(context, value, NULL);
        gsize maxStringSize = JSStringGetMaximumUTF8CStringSize(jsString);

        std::string result(maxStringSize - 1, '\0');
        size_t exactStringSize = JSStringGetUTF8CString(jsString, result.data(), maxStringSize) - 1;

        JSStringRelease(jsString);

        result.resize(exactStringSize);

        return std::make_optional<std::string>(std::move(result));
	}
}

#endif
