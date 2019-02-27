#import <Cocoa/Cocoa.h>
#include "platform.h"
#include <utility>

void* DeskGapPlatform::InitUIThread() {
    [NSApplication sharedApplication];
    return nullptr;
}

void DeskGapPlatform::InitNodeThread() { }

void DeskGapPlatform::Run() {
    CFRunLoopRun();
}

namespace {
    NSString* pathOfResource(const std::vector<const char*>& paths) {
        NSMutableArray<NSString*>* allPaths = [NSMutableArray arrayWithCapacity: paths.size() + 1];
        [allPaths addObject: [[NSBundle mainBundle] resourcePath]];
        for (const char* path: paths) {
            [allPaths addObject: [NSString stringWithUTF8String: path]];
        }
        return [NSString pathWithComponents: allPaths];
    }
}

std::string DeskGapPlatform::PathOfResource(const std::vector<const char*>& paths) {
    return std::string([pathOfResource(paths) UTF8String]);
}

bool DeskGapPlatform::ResourceExists(const std::vector<const char*>& paths) {
    return [[NSFileManager defaultManager] fileExistsAtPath: pathOfResource(paths)];
}
