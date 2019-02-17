#ifndef mac_browserwindow_impl_h
#define mac_browserwindow_impl_h

#import <Cocoa/Cocoa.h>

#include "../window/browser_window.h"

struct DeskGap::BrowserWindow::Impl {
    NSWindow* nsWindow;
    id<NSWindowDelegate> nsWindowDelegate;
    NSMutableArray<NSVisualEffectView*>* effectViews;
    NSMutableArray<NSLayoutConstraint*>* effectViewLayoutConstraints = [NSMutableArray new];

    const WebView* webview;

    void SetStyleMask(bool on, NSWindowStyleMask flag);
    void SetTrafficLightsVisible(bool visible);
};

#endif
