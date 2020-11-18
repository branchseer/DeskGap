target_compile_options(deskgap_platform PUBLIC "-fobjc-arc")

target_link_libraries(deskgap_platform PUBLIC "-framework AppKit" "-framework WebKit")

add_library(deskgap_platform_preload_script STATIC ${CMAKE_CURRENT_BINARY_DIR}/dg_preload_mac.c)

bin2code(${CMAKE_CURRENT_LIST_DIR}/dg_preload_mac.js ${CMAKE_CURRENT_BINARY_DIR}/dg_preload_mac.c)

target_sources(deskgap_platform PRIVATE
    cocoa/DeskGapLocalURLSchemeHandler.h
    cocoa/DeskGapLocalURLSchemeHandler.mm
    cocoa/DeskGapWindow.h
    cocoa/DeskGapWindow.m

    util/NSScreen_geo.h

    app.mm
    BrowserWindow.mm
    BrowserWindow_impl.h
    dialog.mm
    menu.mm
    menu_impl.h
    shell.mm
    system_preferences.mm
    dispatch.mm
    exception.mm
    webview.mm
)

target_link_libraries(deskgap_platform PRIVATE deskgap_platform_preload_script)
