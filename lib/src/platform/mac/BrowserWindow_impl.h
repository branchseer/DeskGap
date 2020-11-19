#ifndef DESKGAP_MAC_BROWSER_WINDOW_IMPL_H
#define DESKGAP_MAC_BROWSER_WINDOW_IMPL_H

#import <Cocoa/Cocoa.h>

#include "browser_window.hpp"
#include <optional>

@interface DeskGapBrowserWindowInternal: NSObject <NSWindowDelegate>
-(instancetype)initWithCallbacks: (DeskGap::BrowserWindow::EventCallbacks&) callbacks;
@end

struct DeskGap::BrowserWindow::Impl {
    NSWindow* nsWindow;
    id<NSWindowDelegate> nsWindowDelegate;
    NSMutableArray<NSVisualEffectView*>* effectViews;
    NSMutableArray<NSLayoutConstraint*>* effectViewLayoutConstraints = [NSMutableArray new];
    bool maximizable;

    void SetStyleMask(bool on, NSWindowStyleMask flag);
    void SetTrafficLightsVisible(bool visible);

    TitleBarStyle titleStyle = TitleBarStyle::DEFAULT;
    void UpdateTrafficLightPosition();

    std::optional<NSPoint> trafficLightPosition;

    bool exitingFullScreen = false;
};

#endif
