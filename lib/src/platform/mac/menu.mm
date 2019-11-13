#import <Cocoa/Cocoa.h>
#include <functional>
#include <memory>
#include <unordered_map>
#include <string>
#include "menu.hpp"
#include "menu_impl.h"
#include "./util/string_convert.h"


@interface DeskGapMenuItemTarget: NSObject
-(instancetype)initWithOnClickFunction: (std::function<void()>&) onClick;
- (void)handleAction;
@end

@implementation DeskGapMenuItemTarget {
    std::function<void()> _onClickFunction;
}

-(instancetype)initWithOnClickFunction: (std::function<void()>&) onClick {
    self = [super init];
    if (self) {
        _onClickFunction = std::move(onClick);
    }
    return self;
}

- (void)handleAction {
    _onClickFunction();
}

@end

namespace DeskGap {
    MenuItem::MenuItem(const std::string& role, const Type& type, const Menu* submenu, EventCallbacks&& eventCallbacks): impl_(std::make_unique<Impl>()) {
        impl_->role = role;
        static const std::unordered_map<std::string, SEL> kActionsByRole {
            { "about", @selector(orderFrontStandardAboutPanel:) },
            { "hide", @selector(hide:) },
            { "hideothers", @selector(hideOtherApplications:) },
            { "unhide", @selector(unhideAllApplications:) },
            { "front", @selector(arrangeInFront:) },
            { "undo", @selector(undo:) },
            { "redo", @selector(redo:) },
            { "cut", @selector(cut:) },
            { "copy", @selector(copy:) },
            { "paste", @selector(paste:) },
            { "delete", @selector(delete:) },
            { "pasteandmatchstyle", @selector(pasteAndMatchStyle:) },
            { "selectall", @selector(selectAll:) },
            { "startspeaking", @selector(startSpeaking:) },
            { "stopspeaking", @selector(stopSpeaking:) },
            { "minimize", @selector(performMiniaturize:) },
            { "close", @selector(performClose:) },
            { "zoom", @selector(performZoom:) },
            { "quit", @selector(terminate:) },
            { "togglefullscreen", @selector(toggleFullScreen:) },
            { "toggletabbar", @selector(toggleTabBar:) },
            { "selectnexttab", @selector(selectNextTab:) },
            { "selectprevioustab", @selector(selectPreviousTab:) },
            { "mergeallwindows", @selector(mergeAllWindows:) },
            { "movetabtonewwindow", @selector(moveTabToNewWindow:) },
            { "clearrecentdocuments", @selector(clearRecentDocuments:) },
            { "reload", @selector(reloadFromOrigin:) },
        };


        if (type == Type::SEPARATOR) {
            impl_->ns_menu_item = [NSMenuItem separatorItem];
        }
        else {
            NSMenuItem* item = [[NSMenuItem alloc] init];
            impl_->ns_menu_item = item;

            if (auto roleActionIterator = kActionsByRole.find(role); roleActionIterator != kActionsByRole.end()) {
                [item setAction: roleActionIterator->second];
            }
            else if (role == "toggledevtools") {
                [item setAction: @selector(deskgap_toggleDevTools:)];
            }
            else {
                if (submenu == nullptr) {
                    DeskGapMenuItemTarget* itemTarget = [[DeskGapMenuItemTarget alloc] initWithOnClickFunction: eventCallbacks.onClick];
                    [item setTarget: itemTarget];
                    [item setAction: @selector(handleAction)];
                    impl_->item_target = itemTarget;
                }
                else {
                    NSMenu* cocoaSubmenu = submenu->impl_->ns_menu;
                    if (role == "help") {
                        [NSApp setHelpMenu: cocoaSubmenu];
                    }
                    else if (role == "services") {
                        [NSApp setServicesMenu: cocoaSubmenu];
                    }
                    else if (role == "windowmenu") {
                        [NSApp setWindowsMenu: cocoaSubmenu];
                    }
                    [impl_->ns_menu_item setSubmenu: cocoaSubmenu];
                }
            }
        }
    }

    void MenuItem::SetLabel(const std::string& utf8label) {
        if (impl_->role == "toggledevtools") return;
        [impl_->ns_menu_item setTitle: NSStr(utf8label)];
    }
    
    void MenuItem::SetEnabled(bool enabled) {
        [impl_->ns_menu_item setEnabled: enabled];
    }

    void MenuItem::SetChecked(bool checked) {
        if (impl_->role == "toggledevtools") return;
        [impl_->ns_menu_item setState: checked ? NSControlStateValueOn: NSControlStateValueOff];
    }

    std::string MenuItem::GetLabel() {
        return std::string([[impl_->ns_menu_item title] UTF8String]);
    }

    void MenuItem::SetAccelerator(const std::vector<std::string>& tokens) {
        static const std::unordered_map<std::string, NSEventModifierFlags> kModifierFlagsByToken {
            { "cmd",  NSEventModifierFlagCommand },
            { "ctrl", NSEventModifierFlagControl },
            { "shift", NSEventModifierFlagShift },
            { "option", NSEventModifierFlagOption }
        };

        NSEventModifierFlags modifierFlags = 0;
        NSString* key = @"";

        for (const std::string& token: tokens) {
            if (auto modifierFlagIterator = kModifierFlagsByToken.find(token); modifierFlagIterator != kModifierFlagsByToken.end()) {
                modifierFlags |= modifierFlagIterator->second;
            }
            else {
                key = NSStr(token);
            }
        }
        [impl_->ns_menu_item setKeyEquivalentModifierMask: modifierFlags];
        [impl_->ns_menu_item setKeyEquivalent: key];
    }


    MenuItem::~MenuItem() = default;

    Menu::Menu(const Type& type): impl_(std::make_unique<Impl>()) {
        impl_->ns_menu = [[NSMenu alloc] init];
    }

    void Menu::AppendItem(const MenuItem& menuItem) {
        [impl_->ns_menu addItem: menuItem.impl_->ns_menu_item];
    }

    Menu::~Menu() = default;
}
