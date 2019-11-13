#import "NSScreen+Geometry.h"

@implementation NSScreen (Geometry)
- (NSRect)DeskGap_convertRectToVisiblePortion:(NSRect) rect {
    NSRect visibleFrame = [self visibleFrame];
    rect.origin.x -= visibleFrame.origin.x;
    rect.origin.y -= visibleFrame.origin.y;
    rect.origin.y = visibleFrame.size.height - rect.origin.y - rect.size.height;
    return rect;
}
- (NSRect)DeskGap_convertRectFromVisiblePortion:(NSRect) rect {
    NSRect visibleFrame = [self visibleFrame];
    rect.origin.y = visibleFrame.size.height - rect.origin.y - rect.size.height;
    rect.origin.x += visibleFrame.origin.x;
    rect.origin.y += visibleFrame.origin.y;
    return rect;
}
@end
