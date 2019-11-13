#ifndef DESKGAP_MAC_BROWSER_WINDOW_IMPL_H
#define DESKGAP_MAC_BROWSER_WINDOW_IMPL_H

#import <Cocoa/Cocoa.h>

#include "browser_window.hpp"

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
