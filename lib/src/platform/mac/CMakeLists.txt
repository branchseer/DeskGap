add_library(deskgap_platform STATIC)
target_compile_options(deskgap_platform PUBLIC "-fobjc-arc")
target_link_libraries(deskgap_platform -framework Cocoa -framework WebKit)
target_sources(deskgap_platform PRIVATE
    cocoa/DeskGapLocalURLSchemeHandler.h
    cocoa/DeskGapLocalURLSchemeHandler.mm
    cocoa/DeskGapWindow.h
    cocoa/DeskGapWindow.m

    util/NSScreen+Geometry.h
    util/NSScreen+Geometry.m

    app.mm
    BrowserWindow.mm
    BrowserWindow_impl.h
    dialog.mm
    menu.mm
    menu_impl.h
    shell.mm
    system_preferences.mm
)
