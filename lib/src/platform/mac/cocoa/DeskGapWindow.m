#import "DeskGapWindow.h"

@implementation DeskGapWindow {
    NSEvent* lastLeftMouseDownEvent_;
}

-(void)deskgap_startDragging {
    if (!lastLeftMouseDownEvent_) return;
    [self performWindowDragWithEvent: lastLeftMouseDownEvent_];
}

- (void)sendEvent: (NSEvent*)event {
    [super sendEvent: event];

    switch (event.type) {
    case NSEventTypeLeftMouseDown:
        lastLeftMouseDownEvent_ = event;
        break;
    case NSEventTypeLeftMouseUp:
        lastLeftMouseDownEvent_ = nil;
        break;
    default:
        break;
    }
}

@end
