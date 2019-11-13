#ifndef DeskGapLocalURLSchemeHandler_h
#define DeskGapLocalURLSchemeHandler_h

#import <WebKit/WebKit.h>

API_AVAILABLE(macosx(10.13))
@interface DeskGapLocalURLSchemeHandler : NSObject <WKURLSchemeHandler>
-(void) servePath: (NSString*) path;
@end

#endif
