#import "DeskGapWindow.h"

@implementation DeskGapWindow {
    NSEvent* lastLeftMouseDownEvent_;
    BOOL isDragging_;
}

-(void)deskgap_startDragging {
    if (!lastLeftMouseDownEvent_) return;
    if (@available(macOS 10.11, *)) {
        [self performWindowDragWithEvent: lastLeftMouseDownEvent_];
    }
    else {
        if (!(self.styleMask & NSWindowStyleMaskFullScreen)) {
            isDragging_ = YES;
        }
    }
}

//Reference: https://github.com/electron/electron/blob/6f3c46cc7e36d231e91b2ddc77edb532bdbc78b2/atom/browser/native_browser_view_mac.mm
- (void)sendEvent: (NSEvent*)event {
    [super sendEvent: event];

    switch (event.type) {
    case NSEventTypeLeftMouseDown:
        lastLeftMouseDownEvent_ = event;
        break;
    case NSEventTypeLeftMouseUp:
        lastLeftMouseDownEvent_ = nil;
        isDragging_ = NO;
        break;
    case NSEventTypeLeftMouseDragged:
        if (@available(macOS 10.11, *)) {
            return;
        }
        
        if (!lastLeftMouseDownEvent_ || !isDragging_) {
            return;
        }

        NSPoint initialLocation = [lastLeftMouseDownEvent_ locationInWindow];
        NSPoint currentLocation = [NSEvent mouseLocation];
        NSPoint newOrigin;

        NSRect screenFrame = [[NSScreen mainScreen] frame];
        NSSize screenSize = screenFrame.size;
        NSRect windowFrame = [self frame];
        NSSize windowSize = windowFrame.size;

        newOrigin.x = currentLocation.x - initialLocation.x;
        newOrigin.y = currentLocation.y - initialLocation.y;

        BOOL inMenuBar = (newOrigin.y + windowSize.height) >
                        (screenFrame.origin.y + screenSize.height);
        BOOL screenAboveMainScreen = false;

        if (inMenuBar) {
            for (NSScreen* screen in [NSScreen screens]) {
                NSRect currentScreenFrame = [screen frame];
                BOOL isHigher = currentScreenFrame.origin.y > screenFrame.origin.y;

            // If there's another screen that is generally above the current screen,
            // we'll draw a new rectangle that is just above the current screen. If
            // the "higher" screen intersects with this rectangle, we'll allow drawing
            // above the menubar.
                if (isHigher) {
                    NSRect aboveScreenRect =
                        NSMakeRect(screenFrame.origin.x,
                            screenFrame.origin.y + screenFrame.size.height - 10,
                            screenFrame.size.width, 200);

                    BOOL screenAboveIntersects =
                        NSIntersectsRect(currentScreenFrame, aboveScreenRect);
                    if (screenAboveIntersects) {
                        screenAboveMainScreen = true;
                        break;
                    }
                }
            }
        }

        // Don't let window get dragged up under the menu bar
        if (inMenuBar && !screenAboveMainScreen) {
            newOrigin.y = screenFrame.origin.y +
                (screenFrame.size.height - windowFrame.size.height);
        }

        // Move the window to the new location
        [self setFrameOrigin: newOrigin];
        break;
    default:
        break;
    }
}

@end
