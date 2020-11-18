bin2code(${CMAKE_CURRENT_LIST_DIR}/dg_preload_gtk.js ${CMAKE_CURRENT_BINARY_DIR}/dg_preload_gtk.c)

add_library(deskgap_platform_preload_script STATIC ${CMAKE_CURRENT_BINARY_DIR}/dg_preload_gtk.c)
target_link_libraries(deskgap_platform PRIVATE deskgap_platform_preload_script)

find_package(PkgConfig REQUIRED)
pkg_check_modules(gtk REQUIRED IMPORTED_TARGET gtk+-3.0>=3.18.9)
pkg_check_modules(webkit REQUIRED IMPORTED_TARGET webkit2gtk-4.0>=2.20.5)
target_link_libraries(deskgap_platform PRIVATE PkgConfig::gtk PkgConfig::webkit stdc++fs)

target_sources(deskgap_platform PRIVATE
    app.cpp
    BrowserWindow.cpp
    dialog.cpp
    glib_exception.cpp
    exception.cpp
    menu.cpp
    shell.cpp
    system_preferences.cpp
    ui_dispatch_platform.cpp
    webview.cpp
)
