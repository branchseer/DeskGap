#ifndef DESKGAP_MAC_UTIL_NSSCREEN_GEO_H
#define DESKGAP_MAC_UTIL_NSSCREEN_GEO_H

#import <AppKit/AppKit.h>

inline static NSRect DeskGapNSScreenConvertRectToVisiblePortion(NSScreen* screen, NSRect rect) {
    NSRect visibleFrame = [screen visibleFrame];
    rect.origin.x -= visibleFrame.origin.x;
    rect.origin.y -= visibleFrame.origin.y;
    rect.origin.y = visibleFrame.size.height - rect.origin.y - rect.size.height;
    return rect;
}

inline static NSRect DeskGapNSScreenConvertRectFromVisiblePortion(NSScreen* screen, NSRect rect) {
    NSRect visibleFrame = [screen visibleFrame];
    rect.origin.y = visibleFrame.size.height - rect.origin.y - rect.size.height;
    rect.origin.x += visibleFrame.origin.x;
    rect.origin.y += visibleFrame.origin.y;
    return rect;
}

#endif
