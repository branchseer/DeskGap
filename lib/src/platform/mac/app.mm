#import <Foundation/NSBundle.h>
#import <AppKit/NSApplication.h>

#include <functional>
#include <memory>
#include <cstdlib>
#include <unordered_map>

#include "app.hpp"
#include "./util/string_convert.h"
#include "menu_impl.h"

using std::shared_ptr;
using std::function;
using std::make_shared;

@interface DeskGapAppDelegate: NSObject <NSApplicationDelegate>
-(instancetype)initWithCallbacks: (DeskGap::App::EventCallbacks&) callbacks;
@end

@implementation DeskGapAppDelegate {
    DeskGap::App::EventCallbacks callbacks_;
}

-(instancetype)initWithCallbacks: (DeskGap::App::EventCallbacks&) callbacks {
    self = [super init];
    if (self) {
        callbacks_ = std::move(callbacks);
    }
    return self;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    callbacks_.onReady();
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    return NO;
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender {
    callbacks_.beforeQuit();
    return NSTerminateCancel;
}
@end

namespace DeskGap {
    static DeskGapAppDelegate* appDelegate;

    void App::Init() {
        [NSApplication sharedApplication];
    }

    void App::Run(EventCallbacks&& callbacks) {
        appDelegate = [[DeskGapAppDelegate alloc] initWithCallbacks: callbacks];
        NSApp.delegate = appDelegate;
        [NSApp run];
    }
    
    void App::Exit(int exitCode) {
        std::exit(exitCode);
    }

    std::string App::GetPath(PathName name) {
        static std::unordered_map<PathName, NSSearchPathDirectory> searchPathDirectoryByName {
            { PathName::APP_DATA,  NSApplicationSupportDirectory },
            { PathName::DESKTOP, NSDesktopDirectory },
            { PathName::DOCUMENTS, NSDocumentDirectory },
            { PathName::DOWNLOADS, NSDownloadsDirectory },
            { PathName::MUSIC, NSMusicDirectory },
            { PathName::PICTURES, NSPicturesDirectory },
            { PathName::VIDEOS, NSMoviesDirectory },
        };

        if (name == PathName::TEMP) {
            return CXXStr(NSTemporaryDirectory());
        }
        else if (name == PathName::HOME) {
            return CXXStr(NSHomeDirectory());
        }

        NSSearchPathDirectory searchPathDirectory = searchPathDirectoryByName[name];

        NSURL* pathURL = [[NSFileManager defaultManager]
            URLForDirectory: searchPathDirectory
            inDomain: NSUserDomainMask
            appropriateForURL: nil
            create: YES
            error: nil
        ];

        return CXXStr([pathURL path]);
    }

    std::string App::GetResourcePath(const char* argv0) {
        return std::string(NSBundle.mainBundle.resourcePath.UTF8String);
    }

    void App::SetMenu(std::optional<std::reference_wrapper<Menu>> menu) {
        if (!menu.has_value()) {
            NSApp.menu = nil;
            return;
        }
        NSMenu* cocoaMenu = menu->get().impl_->ns_menu;
        for (NSInteger i = 0; i < cocoaMenu.numberOfItems; ++i) {
            NSMenuItem* item = [cocoaMenu itemAtIndex: i];
            [[item submenu] setTitle: [item title]];
        }
        NSApp.menu = cocoaMenu;
    }
}
