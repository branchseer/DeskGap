#import <Cocoa/Cocoa.h>

@interface NSScreen (Geometry)
- (NSRect)DeskGap_convertRectToVisiblePortion:(NSRect) rect;
- (NSRect)DeskGap_convertRectFromVisiblePortion:(NSRect) rect;
@end
