#import <Cocoa/Cocoa.h>
#include <functional>
#include <memory>
#include <string>

#import "./cocoa/DeskGapWindow.h"
#import "./cocoa/DeskGapLocalURLSchemeHandler.h"
#include "webview.hpp"
#include "webview_impl.h"
#include "./util/string_convert.h"

extern char BIN2CODE_DG_PRELOAD_MAC_JS_CONTENT[];
extern int BIN2CODE_DG_PRELOAD_MAC_JS_SIZE;

namespace {
    NSString* const DevToolsAlertSuppressionKey = @"DeskGap.Suppressions.DevToolsAlert";
    NSArray<NSString*>* const ObservedWKWebViewKeyPaths = @[ @"title" ];
    NSString* const StringMessageHandlerName = @"stringMessage";
    NSString* const WindowDragHandlerName = @"windowDrag";
    NSString* const localURLScheme = @"deskgap-local";
}

@interface DeskGapWebView: WKWebView @end
@implementation DeskGapWebView

- (void)deskgap_toggleDevTools: (id)sender {
    BOOL isDevToolsEnabled = ![[self.configuration.preferences valueForKey: @"developerExtrasEnabled"] boolValue];
    [self.configuration.preferences setValue: [NSNumber numberWithBool: isDevToolsEnabled] forKey: @"developerExtrasEnabled"];

    if (isDevToolsEnabled) {
        if (!self.window) return;
        NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
        if ([defaults boolForKey: DevToolsAlertSuppressionKey]) {
            return;
        }

        NSAlert* alert = [[NSAlert alloc] init];
        [alert setMessageText: @"Developer Tools Enabled"];
        [alert setInformativeText: @"Right click the page and select “Inspect Element” To show the tools."];
        [alert setShowsSuppressionButton: YES];
        
        [alert beginSheetModalForWindow: self.window completionHandler: ^(NSModalResponse) {
            if (alert.suppressionButton.state == NSControlStateValueOn) {
                [defaults setBool: YES forKey: DevToolsAlertSuppressionKey];
            }
        }];
    }
}

- (BOOL)validateMenuItem:(NSMenuItem*)item {
    if (item.action == @selector(deskgap_toggleDevTools:)) {
        BOOL isDevToolsEnabled = [[self.configuration.preferences valueForKey: @"developerExtrasEnabled"] boolValue];
        [item setTitle: isDevToolsEnabled? @"Disable Developer Tools": @"Enable Developer Tools"];
    }
    return YES;
}

//This only works in 10.11+. Haven't figured a way to hide the Reload item in 10.10
- (void)willOpenMenu:(NSMenu *)menu withEvent:(NSEvent *)event {
    static NSArray<NSString*>* identifiersToBeDeleted = @[
        @"WKMenuItemIdentifierGoBack",
        @"WKMenuItemIdentifierGoForward",
        @"WKMenuItemIdentifierReload"
    ];
    for (NSInteger i = 0; i < menu.numberOfItems; ++i) {
        NSMenuItem* item = [menu itemAtIndex: i];
        if ([identifiersToBeDeleted containsObject: item.identifier]) {
            [item setHidden: YES];
        }
        
    }
    [super willOpenMenu: menu withEvent: event];
}

@end


@interface DeskGapWebViewDelegate: NSObject <WKNavigationDelegate, WKScriptMessageHandler, WKUIDelegate>
-(instancetype)initWithCallbacks: (DeskGap::WebView::EventCallbacks&) callbacks;
@end

@implementation DeskGapWebViewDelegate {
    DeskGap::WebView::EventCallbacks callbacks_;
}
-(instancetype)initWithCallbacks: (DeskGap::WebView::EventCallbacks&) callbacks {
    self = [super init];
    if (self) {
        callbacks_ = std::move(callbacks);
    }
    return self;
}

- (void)webView:(WKWebView *)webView didFailNavigation:(WKNavigation *)navigation withError:(NSError *)error {
    callbacks_.didFinishLoad();
}

- (void)webView:(WKWebView *)webView didFinishNavigation:(WKNavigation *)navigation {
    callbacks_.didFinishLoad();
}

- (void)webView:(WKWebView *)webView didFailProvisionalNavigation:(WKNavigation *)navigation withError:(NSError *)error {
    callbacks_.didFinishLoad();
}

- (void)userContentController:(WKUserContentController *) userContentController didReceiveScriptMessage:(WKScriptMessage *)message {
    if ([message.name isEqualToString: StringMessageHandlerName]) {
        callbacks_.onStringMessage([[message body] UTF8String]);
    }
    else if ([message.name isEqualToString: WindowDragHandlerName]) {
        NSWindow* window = message.webView.window;
        if (window) {
            [(DeskGapWindow*)window deskgap_startDragging];
        }
    }
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary<NSKeyValueChangeKey, id> *)change context:(void *)context {
    if ([keyPath isEqualToString: @"title"]) {
        callbacks_.onPageTitleUpdated([(NSString*)(change[NSKeyValueChangeNewKey]) UTF8String]);
    }
}

- (void)webView:(WKWebView *)webView
    runOpenPanelWithParameters:(WKOpenPanelParameters *)parameters
    initiatedByFrame:(WKFrameInfo *)frame
    completionHandler:(void (^)(NSArray<NSURL *> *URLs))completionHandler
    API_AVAILABLE(macosx(10.12))
{
    NSOpenPanel* openPanel = [NSOpenPanel openPanel];
    [openPanel setAllowsMultipleSelection: parameters.allowsMultipleSelection];
    if (@available(macOS 10.13.4, *)) {
        [openPanel setCanChooseDirectories: parameters.allowsDirectories];
    }

    [openPanel beginSheetModalForWindow: [webView window] completionHandler: ^(NSModalResponse result) {
        if (result == NSModalResponseOK) {
            completionHandler([openPanel URLs]);
        }
        else {
            completionHandler(nil);
        }
    }];
}

@end


namespace DeskGap {
    void WebView::Impl::ServePath(NSString* path) {
        if (@available(macOS 10.13, *)) {
            [(DeskGapLocalURLSchemeHandler*)localURLSchemeHandler servePath: path];
        }
    }

    WebView::WebView(EventCallbacks&& callbacks, const std::string& preloadScriptString): impl_(std::make_unique<Impl>()) {
        std::string preloadScript = std::string(BIN2CODE_DG_PRELOAD_MAC_JS_CONTENT, BIN2CODE_DG_PRELOAD_MAC_JS_SIZE) + preloadScriptString;
        DeskGapWebViewDelegate* webviewDelegate = [[DeskGapWebViewDelegate alloc] initWithCallbacks: callbacks];

        WKWebViewConfiguration* configuration = [[WKWebViewConfiguration alloc] init];


        if (@available(macOS 10.13, *)) {
            //WKURLSchemeHandlers enables web workers and ajax requesting local files in 10.13+
            DeskGapLocalURLSchemeHandler* handler = [DeskGapLocalURLSchemeHandler new];
            impl_->localURLSchemeHandler = handler;
            [configuration setURLSchemeHandler: handler forURLScheme: localURLScheme];
        }

        if (@available(macOS 10.11, *)) {
            //This makes web workers work in 10.11 and 10.12
            [configuration.preferences setValue: [NSNumber numberWithBool: YES] forKey: @"allowFileAccessFromFileURLs"];
        }

        for (NSString* handlerName in @[StringMessageHandlerName, WindowDragHandlerName]) {
            [configuration.userContentController
                addScriptMessageHandler: webviewDelegate
                name: handlerName
            ];
        }
        
        [configuration.userContentController
            addUserScript: [[WKUserScript alloc]
                initWithSource: NSStr(preloadScript)
                injectionTime: WKUserScriptInjectionTimeAtDocumentStart
                forMainFrameOnly: YES
            ]
        ];

        WKWebView* wkWebView = [[DeskGapWebView alloc] initWithFrame: CGRectZero configuration: configuration];

        if (@available(macOS 10.12, *)) {
            [wkWebView setValue: @NO forKey:@"drawsBackground"];
        }
        else {
            [wkWebView setValue: @YES forKey:@"drawsTransparentBackground"];
        }

        [wkWebView setNavigationDelegate: webviewDelegate];
        [wkWebView setUIDelegate: webviewDelegate];

        for (NSString* keyPath in ObservedWKWebViewKeyPaths) {
            [wkWebView addObserver: webviewDelegate forKeyPath: keyPath options: NSKeyValueObservingOptionNew context: nil];
        }

        impl_->wkWebView = wkWebView;
        impl_->webViewDelegate = webviewDelegate;
    }

    void WebView::LoadLocalFile(const std::string& path) {
        impl_->ServePath(nil);
        if (@available(macOS 10.13, *)) {
            NSString* cocoaPath = NSStr(path);
            NSString* folder = [cocoaPath stringByDeletingLastPathComponent];
            impl_->ServePath(folder);

            NSString* filename = [cocoaPath lastPathComponent];
            NSString* encodedFilename = [filename stringByAddingPercentEncodingWithAllowedCharacters: [NSCharacterSet URLPathAllowedCharacterSet]];

            NSURL* localFileRequestURL = [NSURL URLWithString: [NSString stringWithFormat: @"%@://host/%@", localURLScheme, encodedFilename]];
            [impl_->wkWebView loadRequest:[NSURLRequest requestWithURL: localFileRequestURL]];
        } else
        if (@available(macOS 10.11, *)) {
            static NSURL* rootURL = [NSURL fileURLWithPath: @"/"];
            NSURL* fileURL = [NSURL fileURLWithPath: NSStr(path)];
            [impl_->wkWebView
                loadFileURL: fileURL
                allowingReadAccessToURL: rootURL
            ];
        }
        else {
            NSURL* fileURL = [NSURL fileURLWithPath: NSStr(path)];
            [impl_->wkWebView loadRequest: [NSURLRequest requestWithURL: fileURL]];
        }
    }

    void WebView::LoadRequest(
        const std::string& method,
        const std::string& urlString,
        const std::vector<HTTPHeader>& headers,
        const std::optional<std::string>& body
    ) {
        impl_->ServePath(nil);
        NSString* urlNSString = NSStr(urlString);
        NSURL* url = [NSURL URLWithString: urlNSString];
        if (!url) {
            @throw [NSException
                exceptionWithName: NSInvalidArgumentException
                reason: [NSString stringWithFormat: @"The URL string was malformed: %@", urlNSString]
                userInfo: nil
            ];
        }
        NSMutableURLRequest* request = [NSMutableURLRequest requestWithURL: url];
        [request setHTTPMethod: NSStr(method)];
        if (body.has_value()) {
            [request setHTTPBody: [NSData dataWithBytes: body->data() length: body->size()]];
        }
        for (const HTTPHeader& header: headers) {
            [request
                addValue: NSStr(header.value)
                forHTTPHeaderField: NSStr(header.field)
            ];
        }
        [impl_->wkWebView loadRequest: request];
    }

    void WebView::Reload() {
        [impl_->wkWebView reloadFromOrigin];
    }

    void WebView::ExecuteJavaScript(const std::string& scriptString, std::optional<JavaScriptExecutionCallback>&& optionalCallback) {
        void (^scriptCompletionHandler)(id, NSError *error) = nil;
        if (optionalCallback.has_value()) {
            JavaScriptExecutionCallback callback = std::move(*optionalCallback);
            scriptCompletionHandler = ^(id result, NSError *error) {
                if (error) {
                    NSString* errorString = nil;
                    NSMutableDictionary* errorUserInfo = [[error userInfo] mutableCopy];
                    if (errorUserInfo) {
                        NSURL* sourceURL = [errorUserInfo valueForKey: @"WKJavaScriptExceptionSourceURL"];
                        if (sourceURL) {
                            [errorUserInfo setObject: [sourceURL absoluteString] forKey: @"WKJavaScriptExceptionSourceURL"];
                        }
                        NSData* errorInfoJSONData = nil;
                        @try {
                            errorInfoJSONData = [NSJSONSerialization dataWithJSONObject: errorUserInfo options: kNilOptions error: nil];
                        }
                        @catch (NSException *exception) { }
                        errorString = [[NSString alloc] initWithData: errorInfoJSONData encoding: NSUTF8StringEncoding];
                    }
                    
                    if (!errorString) {
                        errorString = [error localizedDescription];
                    }
                    
                    callback(std::make_optional<std::string>([errorString UTF8String]));
                }
                else {
                    callback(std::nullopt);
                }
            };
        }


        [impl_->wkWebView 
            evaluateJavaScript: NSStr(scriptString)
            completionHandler: scriptCompletionHandler
        ];
    }


    void WebView::SetDevToolsEnabled(bool enabled) {
        [impl_->wkWebView.configuration.preferences setValue: [NSNumber numberWithBool: enabled] forKey: @"developerExtrasEnabled"];
    }
    WebView::~WebView() = default;

    WebView::Impl::~Impl() {
        for (NSString* keyPath in ObservedWKWebViewKeyPaths) {
            [wkWebView removeObserver: webViewDelegate forKeyPath: keyPath];
        }
    }
}
