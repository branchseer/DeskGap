target_compile_options(deskgap_platform PUBLIC "-fobjc-arc")

# "-weak_framework CoreFoundation" could solve some dyld errors on old versions of macOS
target_link_libraries(deskgap_platform PUBLIC "-framework AppKit" "-framework WebKit" "-weak_framework CoreFoundation")
target_link_options(deskgap_platform PUBLIC "-ObjC")

add_library(deskgap_platform_preload_script OBJECT ${CMAKE_CURRENT_BINARY_DIR}/dg_preload_mac.c)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dg_preload_mac.c
    COMMAND ${DG_BIN2CODE} ${CMAKE_CURRENT_LIST_DIR}/dg_preload_mac.js ${CMAKE_CURRENT_BINARY_DIR}/dg_preload_mac.c
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/dg_preload_mac.js
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

target_link_libraries(deskgap_platform PRIVATE deskgap_platform_preload_script)
