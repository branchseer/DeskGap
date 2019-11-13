#import <Foundation/Foundation.h>
#import <WebKit/WebKit.h>
#import "DeskGapLocalURLSchemeHandler.h"
#include "../util/string_convert.h"
#include "../../../utils/mime.hpp"

namespace {
    API_AVAILABLE(macosx(10.13))
    void respond404(id<WKURLSchemeTask> urlSchemeTask) {
        [urlSchemeTask didReceiveResponse: [[NSHTTPURLResponse alloc]
            initWithURL: urlSchemeTask.request.URL
            statusCode: 404
            HTTPVersion:@"HTTP/1.1" headerFields:@{}
        ]];
        [urlSchemeTask didFinish];
    }
}



@implementation DeskGapLocalURLSchemeHandler {
    NSString* path_;
}

-(void) servePath: (NSString*) path {
    path_ = path;
}
- (void)webView:(WKWebView *)webView startURLSchemeTask:(id<WKURLSchemeTask>)urlSchemeTask {
    if (!path_) {
        respond404(urlSchemeTask);
        return;
    }
    
    NSString* filePath = [path_ stringByAppendingPathComponent: urlSchemeTask.request.URL.path];
    NSData* fileContent = [NSData dataWithContentsOfFile:filePath];
    if (!fileContent) {
        return respond404(urlSchemeTask);
    }

    NSString* mimeType = NSStr(DeskGap::GetMimeTypeOfExtension(CXXStr(urlSchemeTask.request.URL.pathExtension)));
    
    [urlSchemeTask didReceiveResponse: [[NSURLResponse alloc]
        initWithURL: urlSchemeTask.request.URL
        MIMEType: mimeType
        expectedContentLength: [fileContent length]
        textEncodingName: nil
    ]];
    [urlSchemeTask didReceiveData: fileContent];
    [urlSchemeTask didFinish];
    
}
- (void)webView:(WKWebView *)webView stopURLSchemeTask:(id<WKURLSchemeTask>)urlSchemeTask { }
@end

