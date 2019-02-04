#import <Cocoa/Cocoa.h>
#include <functional>
#include <memory>
#include <cstdlib>
#include <unordered_map>

#include "../app/app.h"
#include "./util/string_convert.h"
#include "menu_impl.h"

using std::shared_ptr;
using std::function;
using std::make_shared;

@interface DeskGapAppDelegate: NSObject <NSApplicationDelegate>
-(instancetype)initWithCallbacks: (const DeskGap::App::EventCallbacks&) callbacks;
@end

@implementation DeskGapAppDelegate {
    DeskGap::App::EventCallbacks callbacks_;
}

-(instancetype)initWithCallbacks: (const DeskGap::App::EventCallbacks&) callbacks {
    self = [super init];
    if (self) {
        callbacks_ = callbacks;
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
    struct App::Impl {
        DeskGapAppDelegate* appDelegate;
        NSApplication* app;
    };

    App::App(const EventCallbacks& callbacks): impl_(std::make_unique<Impl>()) {
        impl_->appDelegate = [[DeskGapAppDelegate alloc] initWithCallbacks: callbacks];
        impl_->app = [NSApplication sharedApplication];
        [impl_->app setDelegate: impl_->appDelegate];   
    }
    
    void App::Run() {
        NSApplication* app = impl_->app;
        CFRunLoopPerformBlock(CFRunLoopGetMain(), kCFRunLoopCommonModes, ^{
            [app run];
        });
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
            return StdStr(NSTemporaryDirectory());
        }
        else if (name == PathName::HOME) {
            return StdStr(NSHomeDirectory());
        }

        NSSearchPathDirectory searchPathDirectory = searchPathDirectoryByName[name];

        NSURL* pathURL = [[NSFileManager defaultManager]
            URLForDirectory: searchPathDirectory
            inDomain: NSUserDomainMask
            appropriateForURL: nil
            create: YES
            error: nil
        ];

        return StdStr([pathURL path]);
    }

    void App::SetMenu(std::optional<std::reference_wrapper<Menu>> menu) {
        if (!menu.has_value()) {
            [impl_->app setMenu: nil];
            return;
        }
        NSMenu* cocoaMenu = menu->get().impl_->ns_menu;
        for (NSInteger i = 0; i < cocoaMenu.numberOfItems; ++i) {
            NSMenuItem* item = [cocoaMenu itemAtIndex: i];
            [[item submenu] setTitle: [item title]];
        }
        [impl_->app setMenu: cocoaMenu];
    }

    
    App::~App() = default;
}
