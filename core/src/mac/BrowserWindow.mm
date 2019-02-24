#import <Cocoa/Cocoa.h>
#include <cmath>
#include <unordered_map>
#include "./BrowserWindow_impl.h"
#import "./util/NSScreen+Geometry.h"
#import "./cocoa/DeskGapWindow.h"
#include "../window/browser_window.h"
#include "./webview_impl.h"
#include "./menu_impl.h"
#include "./util/string_convert.h"


@interface DeskGapBrowserWindowDelegate: NSObject <NSWindowDelegate>
-(instancetype)initWithCallbacks: (DeskGap::BrowserWindow::EventCallbacks&) callbacks;
@end

@implementation DeskGapBrowserWindowDelegate {
    DeskGap::BrowserWindow::EventCallbacks _callbacks;
}

-(instancetype)initWithCallbacks: (DeskGap::BrowserWindow::EventCallbacks&) callbacks {
    self = [super init];
    if (self) {
        _callbacks = std::move(callbacks);
    }
    return self;
}
- (void)windowDidBecomeKey:(NSNotification *)notification {
    _callbacks.onFocus();
}
- (void)windowDidResignKey:(NSNotification *)notification {
    _callbacks.onBlur();
}
- (void)windowDidResize:(NSNotification *)notification {
    _callbacks.onResize();
}
- (void)windowDidMove:(NSNotification *)notification {
    _callbacks.onMove();
}
- (BOOL)windowShouldClose:(NSWindow *)sender {
    _callbacks.onClose();
    return NO;
}
@end


@interface DeskGapWindowContentView: NSView @end
@implementation DeskGapWindowContentView
- (BOOL)isFlipped { return YES; }
@end

namespace DeskGap {
    
    //Reference: https://github.com/electron/electron/blob/5ae3d1a1b2dbe11d3091d366467591d9cb21fdfe/atom/browser/native_window_mac.mm#L1479
    void BrowserWindow::Impl::SetStyleMask(bool on, NSWindowStyleMask flag) {
        bool wasMaximizable = [[nsWindow standardWindowButton:NSWindowZoomButton] isEnabled];

        if (on) {
            nsWindow.styleMask |= flag;
        }
        else {
            nsWindow.styleMask &= ~flag;
        }

        [[nsWindow standardWindowButton: NSWindowZoomButton] setEnabled: wasMaximizable];
    }

    void BrowserWindow::Impl::SetTrafficLightsVisible(bool visible) {
        for (NSWindowButton windowButton: { NSWindowCloseButton, NSWindowMiniaturizeButton, NSWindowZoomButton }) {
            [[nsWindow standardWindowButton: windowButton] setHidden: !visible];
        }
    }
    
    BrowserWindow::BrowserWindow(const WebView& webview, EventCallbacks&& callbacks): impl_(std::make_unique<Impl>()) {
        NSWindow *window = [[DeskGapWindow alloc]
            initWithContentRect: NSZeroRect
            styleMask: NSWindowStyleMaskTitled
            backing: NSBackingStoreBuffered defer: NO
        ];

        [window setReleasedWhenClosed: NO];

        [window setBackgroundColor: [NSColor whiteColor]];

        if (@available(macOS 10.12, *)) {
            [window setTabbingMode: NSWindowTabbingModeDisallowed];
        }

        DeskGapBrowserWindowDelegate* windowDelegate = [[DeskGapBrowserWindowDelegate alloc]
            initWithCallbacks: callbacks
        ];

        [window setDelegate: windowDelegate];

        {
            NSView* contentView = [[DeskGapWindowContentView alloc] init];
            [window setContentView:contentView];
            {
                NSView* wkWebViewContainer = [[NSView alloc] init]; //WKWebView must be in a non-flipped view, or the devtools won't be laid out correctly.
                [contentView addSubview: wkWebViewContainer];
                [wkWebViewContainer setAutoresizingMask: NSViewWidthSizable | NSViewHeightSizable];
                [wkWebViewContainer setFrame: [contentView bounds]];
                {
                    WKWebView* wkWebView = webview.impl_->wkWebView;
                    [wkWebViewContainer addSubview: wkWebView];
                    [wkWebView setAutoresizingMask: NSViewWidthSizable | NSViewHeightSizable];
                    [wkWebView setFrame: [wkWebViewContainer bounds]];
                }
            }
        }

        impl_->nsWindow = window;
        impl_->nsWindowDelegate = windowDelegate;
        impl_->webview = &webview;
        impl_->effectViews = [NSMutableArray new];
    }

    void BrowserWindow::SetMaximizable(bool maximizable) {
        [[impl_->nsWindow standardWindowButton: NSWindowZoomButton] setEnabled: maximizable];
    }
    void BrowserWindow::SetMinimizable(bool minimizable) {
        impl_->SetStyleMask(minimizable, NSWindowStyleMaskMiniaturizable);
    }
    void BrowserWindow::SetResizable(bool resizable) {
        impl_->SetStyleMask(resizable, NSWindowStyleMaskResizable);
    }

    void BrowserWindow::SetHasFrame(bool hasFrame) {
        impl_->SetTrafficLightsVisible(hasFrame);

        NSWindow* window = impl_->nsWindow;

        [window setTitleVisibility: hasFrame ? NSWindowTitleVisible: NSWindowTitleHidden];
        [window setTitlebarAppearsTransparent: !hasFrame];
        impl_->SetStyleMask(!hasFrame, NSWindowStyleMaskFullSizeContentView);

        if (!hasFrame) {
            [window setToolbar: nil];
        }
    }
    void BrowserWindow::SetClosable(bool closable) {
        impl_->SetStyleMask(closable, NSWindowStyleMaskClosable);
    }

    void BrowserWindow::Minimize() {
        [impl_->nsWindow miniaturize: nil];
    }

    namespace {
        template<typename Key, typename Value>
        Value getOrDefault(const std::unordered_map<Key, Value>& map, const Key& key, Value defaultValue) {
            if (auto it = map.find(key); it != map.end()) {
                return it->second;
            }
            else {
                return defaultValue;
            }
        }
    }

    void BrowserWindow::SetVibrancies(const std::vector<Vibrancy>& vibrancies) {
        static bool isMapInitialized = false;
        static std::unordered_map<std::string, NSVisualEffectMaterial> materialsByString {
            { "appearance-based", NSVisualEffectMaterialAppearanceBased },
            { "light", NSVisualEffectMaterialLight },
            { "dark", NSVisualEffectMaterialDark },
            { "medium-light", NSVisualEffectMaterialMediumLight },
            { "ultra-dark", NSVisualEffectMaterialUltraDark },
            { "titlebar", NSVisualEffectMaterialTitlebar },
            { "selection", NSVisualEffectMaterialSelection }
        };
        if (!isMapInitialized) {
            isMapInitialized = true;
            if (@available(macOS 10.11, *)) {
                materialsByString.insert({
                    { "menu", NSVisualEffectMaterialMenu },
                    { "popover", NSVisualEffectMaterialPopover},
                    { "sidebar", NSVisualEffectMaterialSidebar }
                });
            }
            if (@available(macOS 10.14, *)) {
                materialsByString.insert({
                    { "header-view", NSVisualEffectMaterialHeaderView },
                    { "sheet", NSVisualEffectMaterialSheet},
                    { "window-background", NSVisualEffectMaterialWindowBackground },
                    { "hud-window", NSVisualEffectMaterialHUDWindow },
                    { "full-screen-ui", NSVisualEffectMaterialFullScreenUI },
                    { "tool-tip", NSVisualEffectMaterialToolTip },
                    { "content-background", NSVisualEffectMaterialContentBackground },
                    { "under-window-background", NSVisualEffectMaterialUnderWindowBackground },
                    { "under-page-background", NSVisualEffectMaterialUnderPageBackground },
                });
            }
        }

        static std::unordered_map<std::string, NSVisualEffectBlendingMode> blendingModesByString {
            { "behind-window", NSVisualEffectBlendingModeBehindWindow },
            { "within-window", NSVisualEffectBlendingModeWithinWindow },
        };
        static std::unordered_map<std::string, NSVisualEffectState> statesByString {
            { "follows-window", NSVisualEffectStateFollowsWindowActiveState },
            { "active", NSVisualEffectStateActive },
            { "inactive", NSVisualEffectStateInactive },
        };

        static std::unordered_map<std::string, NSLayoutAttribute> layoutAttributesByString {
            { "left", NSLayoutAttributeLeft },
            { "right", NSLayoutAttributeRight },
            { "top", NSLayoutAttributeTop },
            { "bottom", NSLayoutAttributeBottom },
            { "width", NSLayoutAttributeWidth },
            { "height", NSLayoutAttributeHeight }
        };

        NSView* contentView = [impl_->nsWindow contentView];
        
        //NSVisualEffectViews are cached in impl_->effectViews
        NSInteger effectViewsToAdd = vibrancies.size() - [impl_->effectViews count];
        for (NSInteger i = 0; i < effectViewsToAdd; ++i) {
            NSVisualEffectView* effectView = [[NSVisualEffectView alloc] init];

            [effectView setTranslatesAutoresizingMaskIntoConstraints: NO];
            [impl_->effectViews addObject: effectView];
        }

        // Deactivating previous constraints
        [NSLayoutConstraint deactivateConstraints: impl_->effectViewLayoutConstraints];
        [impl_->effectViewLayoutConstraints removeAllObjects];

        for (NSUInteger i = 0; i < [impl_->effectViews count]; ++i) {
            NSVisualEffectView* effectView = [impl_->effectViews objectAtIndex: i];
            [effectView removeFromSuperview];

            if (i < vibrancies.size()) {
                const Vibrancy& v = vibrancies[i];

                [effectView setMaterial: getOrDefault(materialsByString, v.material, NSVisualEffectMaterialAppearanceBased)];
                [effectView setBlendingMode: getOrDefault(blendingModesByString, v.blendingMode, NSVisualEffectBlendingModeBehindWindow)];
                [effectView setState: getOrDefault(statesByString, v.state, NSVisualEffectStateFollowsWindowActiveState)];

                for (const auto& constraint: v.constraints) {
                    NSLayoutAttribute attr1, attr2;
                    CGFloat multiplier, constant;
                    NSView* item2;
                    if (auto it = layoutAttributesByString.find(constraint.attribute); it != layoutAttributesByString.end()) {
                        attr1 = it->second;
                    }
                    else {
                        continue;
                    }

                    if (constraint.valueUnit == Vibrancy::Constraint::Unit::POINT) {
                        if (attr1 == NSLayoutAttributeWidth || attr1 == NSLayoutAttributeHeight) {
                            item2 = nil;
                            attr2 = NSLayoutAttributeNotAnAttribute;
                            multiplier = 0;
                            constant = constraint.value;
                        }
                        else {
                            item2 = contentView;
                            attr2 = attr1;
                            multiplier = 1;
                            if (attr1 == NSLayoutAttributeLeft || attr1 == NSLayoutAttributeTop) {
                                constant = constraint.value;
                            }
                            else {
                                constant = -constraint.value;
                            }
                        }
                    }
                    else { //Percentage
                        item2 = contentView;
                        if (attr1 == NSLayoutAttributeLeft || attr1 == NSLayoutAttributeRight) {
                            attr2 = NSLayoutAttributeWidth;
                        }
                        else if (attr1 == NSLayoutAttributeTop || attr1 == NSLayoutAttributeBottom) {
                            attr2 = NSLayoutAttributeHeight;
                        }
                        else {//attr1 is width or height.
                            attr2 = attr1;
                        }
                        multiplier = constraint.value / 100;
                        constant = 0;
                    }

                    NSLayoutConstraint* layoutConstraint = [NSLayoutConstraint
                        constraintWithItem: effectView attribute: attr1
                        relatedBy: NSLayoutRelationEqual
                        toItem: item2 attribute: attr2
                        multiplier: multiplier constant:constant
                    ];

                    [impl_->effectViewLayoutConstraints addObject: layoutConstraint];
                }

                [contentView
                    addSubview: effectView
                    positioned: [effectView blendingMode] == NSVisualEffectBlendingModeBehindWindow ? NSWindowBelow: NSWindowAbove
                    relativeTo: nil
                ];
            }
        }

        [NSLayoutConstraint activateConstraints: impl_->effectViewLayoutConstraints];
    }


    void BrowserWindow::Show() {
        [impl_->nsWindow makeKeyAndOrderFront: nil];
    }

    void BrowserWindow::Center() {
        [impl_->nsWindow center];
    }

    void BrowserWindow::SetTitle(const std::string& utf8title) {
        [impl_->nsWindow setTitle: NSStr(utf8title)];
    }

    void BrowserWindow::SetSize(int width, int height, bool animate) {
        NSWindow* window = impl_->nsWindow;
        NSScreen* screen = [window screen];
        if (!screen) {
            screen = [NSScreen mainScreen];
        }

        NSRect flippedFrame = [screen DeskGap_convertRectToVisiblePortion: [window frame]];

        flippedFrame.size.width = width;
        flippedFrame.size.height = height;

        [window setFrame: [screen DeskGap_convertRectFromVisiblePortion: flippedFrame] display: YES animate: animate];
    }

    void BrowserWindow::SetMaximumSize(int width, int height) {
        NSWindow* window = impl_->nsWindow;
        if (width == 0 && height == 0) {
            [window setMaxSize: NSMakeSize(FLT_MAX, FLT_MAX)];
        }
        else {
            [window setMaxSize: NSMakeSize(width, height)];
        }
    }
    void BrowserWindow::SetMinimumSize(int width, int height) {
        [impl_->nsWindow setMinSize: NSMakeSize(width, height)];
    }


    void BrowserWindow::SetPosition(int x, int y, bool animate) {
        NSWindow* window = impl_->nsWindow;
        NSScreen* screen = [window screen];
        if (!screen) {
            screen = [NSScreen mainScreen];
        }

        NSRect flippedFrame = [screen DeskGap_convertRectToVisiblePortion: [window frame]];

        flippedFrame.origin.x = x;
        flippedFrame.origin.y = y;

        [window setFrame: [screen DeskGap_convertRectFromVisiblePortion: flippedFrame] display: YES animate: animate];
    }

    std::array<int, 2> BrowserWindow::GetSize() {
        NSRect windowFrame = [impl_->nsWindow frame];
        return { 
            (int)std::round(NSWidth(windowFrame)), 
            (int)std::round(NSHeight(windowFrame))
        };
    }
    std::array<int, 2> BrowserWindow::GetPosition() {
        NSWindow* window = impl_->nsWindow;
        NSScreen* screen = [window screen];
        if (!screen) {
            screen = [NSScreen mainScreen];
        }
        NSRect flippedFrame = [screen DeskGap_convertRectToVisiblePortion: [window frame]];
        return { 
            (int)std::round(NSMinX(flippedFrame)), 
            (int)std::round(NSMinY(flippedFrame))
        };
    }

    void BrowserWindow::Destroy() {
        [impl_->nsWindow close];
    }
    void BrowserWindow::Close() {
        [impl_->nsWindow performClose: nil];
    }

    void BrowserWindow::PopupMenu(const Menu& menu, const std::array<int, 2>* location, int positioningItem) {
        NSView* view = [impl_->nsWindow contentView];
        NSMenu* ns_menu = menu.impl_->ns_menu;

        NSPoint popupLocation;
        if (location != nullptr) {
            popupLocation = NSMakePoint((*location)[0], (*location)[1]);
        }
        else {
            popupLocation = [view convertPoint: [impl_->nsWindow mouseLocationOutsideOfEventStream] fromView: nil];
        }

        NSMenuItem* positioningMenuItem = nil;
        if (positioningItem >= 0 && positioningItem < [ns_menu numberOfItems]) {
            positioningMenuItem = [ns_menu itemAtIndex: positioningItem];
        }

        [ns_menu
            popUpMenuPositioningItem: positioningMenuItem
            atLocation: popupLocation
            inView: view
        ];   
    }

    void BrowserWindow::SetTitleBarStyle(TitleBarStyle titleBarStyle) {
        NSWindow* window = impl_->nsWindow;
        NSRect windowFrame = [window frame];

        impl_->SetTrafficLightsVisible(true);
        switch (titleBarStyle) {
        case TitleBarStyle::DEFAULT: {
            [window setTitleVisibility: NSWindowTitleVisible];
            [window setTitlebarAppearsTransparent: NO];
            impl_->SetStyleMask(false, NSWindowStyleMaskFullSizeContentView);
            [window setToolbar: nil];
            break;
        }
        case TitleBarStyle::HIDDEN: {
            [window setTitleVisibility: NSWindowTitleHidden];
            [window setTitlebarAppearsTransparent: YES];
            impl_->SetStyleMask(true, NSWindowStyleMaskFullSizeContentView);
            [window setToolbar: nil];
            break;
        }
        case TitleBarStyle::HIDDEN_INSET: {
            [window setTitleVisibility: NSWindowTitleHidden];
            [window setTitlebarAppearsTransparent: YES];
            impl_->SetStyleMask(true, NSWindowStyleMaskFullSizeContentView);

            NSToolbar* toolbar = [[NSToolbar alloc] initWithIdentifier:@"deskgap.toolbar.titleBarStyling"];
            [toolbar setShowsBaselineSeparator:NO];
            [window setToolbar: toolbar];
            break;
        }
        default:
            break;
        }

        [window setFrame: windowFrame display: YES];
    }

    BrowserWindow::~BrowserWindow() = default;
}
