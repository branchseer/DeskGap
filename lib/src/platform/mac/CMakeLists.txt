target_compile_options(deskgap_platform PUBLIC "-fobjc-arc")
target_link_libraries(deskgap_platform PUBLIC "-framework AppKit" "-framework WebKit")
target_link_options(deskgap_platform PUBLIC "-ObjC")

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dg_preload_mac.c
    COMMAND node ${CMAKE_SOURCE_DIR}/scripts/bin2code.js ${CMAKE_CURRENT_BINARY_DIR}/dg_preload_mac.js ${CMAKE_CURRENT_BINARY_DIR}/dg_preload_mac.c
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/dg_preload_mac.js
)

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
    dispatch.mm
    exception.mm
    webview.mm
)
