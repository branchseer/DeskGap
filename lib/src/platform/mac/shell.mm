#import <Cocoa/Cocoa.h>

#include "shell.hpp"
#include "./util/string_convert.h"

bool DeskGap::Shell::OpenExternal(const std::string& urlString) {
    NSURL* url = [NSURL URLWithString: NSStr(urlString)];
    if (!url) {
        return false;
    }
    return [[NSWorkspace sharedWorkspace] openURL: url];
}

bool DeskGap::Shell::ShowItemInFolder(const std::string& path) {
    return false;
}
