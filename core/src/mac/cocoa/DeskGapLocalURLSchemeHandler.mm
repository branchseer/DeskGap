#import <Foundation/Foundation.h>
#import <WebKit/WebKit.h>
#import "DeskGapLocalURLSchemeHandler.h"

namespace {
    NSDictionary* const mimeTypes = @ {
        @"txt": @"text/plain",
        @"bmp": @"image/bmp",
        @"css": @"text/css",
        @"git": @"image/gif",
        @"htm": @"text/html",
        @"html": @"text/html",
        @"ico": @"image/vnd.microsoft.icon",
        @"jpg": @"image/jpeg",
        @"jpeg": @"image/jpeg",
        @"js": @"text/javascript",
        @"json": @"application/json",
        @"mjs": @"application/javascript",
        @"mp3": @"audio/mpeg",
        @"mpeg": @"video/mpeg",
        @"png": @"image/png",
        @"pdf": @"application/pdf",
        @"svg": @"image/svg+xml",
        @"tif": @"image/tiff",
        @"tiff": @"image/tiff",
        @"tff": @"font/ttf",
        @"wav": @"audio/wav",
        @"weba" :@"audio/webm",
        @"webm" :@"video/webm",
        @"webp" :@"image/webp",
        @"woff" :@"font/woff",
        @"woff2":@"font/woff2",
        @"xhtml":@"application/xhtml+xml",
        @"xml"  :@"application/xml",
    };

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

    NSString* mimeType = [mimeTypes objectForKey:urlSchemeTask.request.URL.pathExtension];
    if (!mimeType) {
        mimeType = @"application/octet-stream";
    }
    
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

