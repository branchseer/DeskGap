#import <Cocoa/Cocoa.h>
#include <memory>
#include <utility>

#include "../dialog/dialog.h"
#include "./BrowserWindow_impl.h"
#include "./util/string_convert.h"



using CommonFileDialogOptions = DeskGap::Dialog::CommonFileDialogOptions;
using FileFilter = CommonFileDialogOptions::FileFilter;

@interface DeskGapFileDialogAccessoryView: NSView
-(instancetype)initWithDialog: (NSSavePanel*)dialog filters: (const std::vector<FileFilter>&)filters;
-(void)selectFilter: (id)sender;
-(void)selectFilterAt: (NSInteger)postiton;
@end

@implementation DeskGapFileDialogAccessoryView {
    __weak NSSavePanel* dialog_;
    NSPopUpButton* popupButton_;
    NSArray<NSArray<NSString*>*>* extensionList_;
}


-(instancetype)initWithDialog: (NSSavePanel*)dialog filters: (const std::vector<FileFilter>&)filters {
    self = [super initWithFrame: NSMakeRect(0.0, 0.0, 200, 32.0)];
    if (self) {
        dialog_ = dialog;
        {
            NSTextField* label = [[NSTextField alloc] initWithFrame: NSMakeRect(0, 0, 60, 22)];
            [label setEditable: NO];
            [label setStringValue: @"Format:"];
            [label setBordered: NO];
            [label setBezeled: NO];
            [label setDrawsBackground: NO];
            [self addSubview: label];
        }
        {
            popupButton_ = [[NSPopUpButton alloc]
                initWithFrame: NSMakeRect(50, 2, 140, 22)
                pullsDown: NO
            ];
            
            [popupButton_ setTarget: self];
            [popupButton_ setAction: @selector(selectFilter:)];

            NSMutableArray<NSArray<NSString*>*>* extensionList = [NSMutableArray arrayWithCapacity: filters.size()];
            for (const FileFilter& filter: filters) {
                [popupButton_ addItemWithTitle: NSStr(filter.name)];

                NSMutableArray* extensions = [NSMutableArray arrayWithCapacity: filter.extensions.size()];
                for (const std::string& extension: filter.extensions) {
                    [extensions addObject: NSStr(extension)];
                }
                [extensionList addObject: extensions];
                
            }
            extensionList_ = extensionList;

            [self addSubview: popupButton_];
        }
    }
    return self;
}
-(void)selectFilter: (id)sender {
    [self selectFilterAt: [popupButton_ indexOfSelectedItem]];
}
-(void)selectFilterAt: (NSInteger)postiton {
    if (!dialog_) return;
    [popupButton_ selectItemAtIndex: postiton];
    NSArray<NSString*>* allowedExtensions = [extensionList_ objectAtIndex: postiton];
    if ([allowedExtensions count] > 0) {
        [dialog_ setAllowedFileTypes: allowedExtensions];
        [dialog_ setAllowsOtherFileTypes: NO];
    }
    else {
        [dialog_ setAllowedFileTypes: nil];
        [dialog_ setAllowsOtherFileTypes: YES];
    }
}

@end

namespace DeskGap {
    void Dialog::ShowErrorBox(const std::string& title, const std::string& content) {
        NSAlert* alert =[NSAlert new];
        [alert setAlertStyle: NSAlertStyleCritical];
        [alert setMessageText: NSStr(title)];
        [alert setInformativeText: NSStr(content)];
        [alert runModal];
    }
    

    struct Dialog::Impl {
        typedef void (^CallbackBlock)(NSModalResponse returnCode);
        static void PresentDialog(std::optional<std::shared_ptr<BrowserWindow>> browserWindow, NSSavePanel* panel, CallbackBlock callbackBlock) {
            if (browserWindow.has_value()) {
                NSWindow* cocoaWindow = browserWindow.value()->impl_->nsWindow;
                [panel beginSheetModalForWindow: cocoaWindow completionHandler: ^(NSModalResponse returnCode) {
                    [panel orderOut: nil];
                    callbackBlock(returnCode);
                }];
            }
            else {
                callbackBlock([panel runModal]);
                // [panel beginWithCompletionHandler: callbackBlock];
            }
        }
    };

    namespace {
        void PrepareDialog(NSSavePanel* panel, const CommonFileDialogOptions& options) {
            [panel setExtensionHidden: NO]; // If extensionHidden is YES, the extension of nameFieldStringValue maybe pruned.
            if (options.title.has_value()) {
                [panel setTitle: NSStr(options.title.value())];
            }

            if (options.filters.empty()) {
                [panel setAllowsOtherFileTypes: YES];
            }
            else {
                DeskGapFileDialogAccessoryView* accessoryView = [[DeskGapFileDialogAccessoryView alloc] initWithDialog: panel filters: options.filters];
                [panel setAccessoryView: accessoryView];
                [accessoryView selectFilterAt: 0];

                if (@available(macOS 10.11, *)) {
                    if ([panel isKindOfClass: [NSOpenPanel class]]) {
                        [(NSOpenPanel*)panel setAccessoryViewDisclosed: YES];
                    }
                }
            }

            if ([panel isKindOfClass: [NSOpenPanel class]]){
                if (options.defaultDirectory.has_value()) {
                    NSMutableArray<NSString*>* defaultPathComponents = [NSMutableArray arrayWithObjects: 
                        NSStr(options.defaultDirectory.value()), nil
                    ];

                    if (options.defaultFilename.has_value()) {
                        [defaultPathComponents addObject: NSStr(options.defaultFilename.value())];
                    }

                    [panel setDirectoryURL: [NSURL fileURLWithPathComponents: defaultPathComponents]];
                }
            }
            else {
                if (options.defaultDirectory.has_value()) {
                    [panel setDirectoryURL: [NSURL fileURLWithPath: NSStr(options.defaultDirectory.value())]];
                }
                if (options.defaultFilename.has_value()) {
                    [panel setNameFieldStringValue: NSStr(options.defaultFilename.value())];
                }
            }

            if (options.buttonLabel.has_value()) {
                [panel setPrompt: NSStr(options.buttonLabel.value())];
            }

            if (options.message.has_value()) {
                [panel setMessage: NSStr(options.message.value())];
            }
        }
    }

    void Dialog::ShowOpenDialog(
        const std::optional<std::shared_ptr<BrowserWindow>>& browserWindow,
        const OpenDialogOptions& options,
        Callback<OpenDialogResult>&& callback
    ) {
        NSOpenPanel* openPanel = [NSOpenPanel new];
        PrepareDialog(openPanel, options.commonOptions);

        [openPanel setCanChooseFiles: (options.properties & OpenDialogOptions::PROPERTY_OPEN_FILE) != 0];
        [openPanel setCanChooseDirectories: (options.properties & OpenDialogOptions::PROPERTY_OPEN_DIRECTORY) != 0];
        [openPanel setAllowsMultipleSelection: (options.properties & OpenDialogOptions::PROPERTY_MULTI_SELECTIONS) != 0];
        [openPanel setShowsHiddenFiles: (options.properties & OpenDialogOptions::PROPERTY_SHOW_HIDDEN_FILES) != 0];
        [openPanel setCanCreateDirectories: (options.properties & OpenDialogOptions::PROPERTY_CREATE_DIRECTORY) != 0];
        [openPanel setResolvesAliases: (options.properties & OpenDialogOptions::PROPERTY_NO_RESOLVE_ALIASES) == 0];
        [openPanel setTreatsFilePackagesAsDirectories: (options.properties & OpenDialogOptions::PROPERTY_TREAT_PACKAGE_AS_DIRECTORY) != 0];

        auto sharedCallback = std::make_shared<Callback<OpenDialogResult>>(std::move(callback));
        Impl::CallbackBlock callbackBlock = ^(NSModalResponse returnCode) {
            Dialog::OpenDialogResult result;
            if (returnCode == NSModalResponseOK) {
                std::vector<std::string> filePaths;
                NSArray<NSURL*>* urls = [openPanel URLs];
                filePaths.reserve([urls count]);
                for (NSURL* url in urls) {
                    if ([url isFileURL]) {
                        filePaths.push_back(StdStr([url path]));
                    }
                }
                result.filePaths.emplace(std::move(filePaths));
            }
            (*sharedCallback)(std::move(result));
        };

        Impl::PresentDialog(browserWindow, openPanel, callbackBlock);        
    }

    void Dialog::ShowSaveDialog(
        const std::optional<std::shared_ptr<BrowserWindow>>& browserWindow,
        const SaveDialogOptions& options,
        Callback<SaveDialogResult>&& callback
    ) {
        NSSavePanel* savePanel = [NSSavePanel new];
        PrepareDialog(savePanel, options.commonOptions);
        if (const auto& nameFieldLabel = options.nameFieldLabel; nameFieldLabel.has_value()) {
            [savePanel setNameFieldLabel: NSStr(nameFieldLabel.value())];
        }
        if (const auto& showsTagField = options.showsTagField; showsTagField.has_value()) {
            [savePanel setShowsTagField: showsTagField.value()];
        }

        auto sharedCallback = std::make_shared<Callback<SaveDialogResult>>(std::move(callback));
        Impl::CallbackBlock callbackBlock = ^(NSModalResponse returnCode) {
            SaveDialogResult result;
            if (returnCode == NSModalResponseOK) {
                result.filePath.emplace(StdStr([savePanel.URL path]));
            }
            (*sharedCallback)(std::move(result));
        };

        Impl::PresentDialog(browserWindow, savePanel, callbackBlock);        
    }
}
