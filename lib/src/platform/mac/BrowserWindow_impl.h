#ifndef DESKGAP_MAC_BROWSER_WINDOW_IMPL_H
#define DESKGAP_MAC_BROWSER_WINDOW_IMPL_H

#import <Cocoa/Cocoa.h>

#include "browser_window.hpp"

struct DeskGap::BrowserWindow::Impl {
    id internal;
    NSWindow* nsWindow_;
};

#endif
