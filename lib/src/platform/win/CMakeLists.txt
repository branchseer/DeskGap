add_library(deskgap_winrt SHARED winrt_webview.cpp)
set_target_properties(deskgap_winrt PROPERTIES
    CXX_STANDARD 17
)
target_include_directories(deskgap_winrt PRIVATE ../../include/deskgap)

target_compile_options(deskgap_winrt
    PUBLIC -GR -await
)

target_compile_definitions(deskgap_winrt
    PUBLIC WIN32_LEAN_AND_MEAN UNICODE _UNICODE
    PRIVATE DESKGAP_EXPORTING
)

add_library(deskgap_platform_trident_preload_script STATIC
    ${CMAKE_CURRENT_BINARY_DIR}/dg_preload_trident.c
)
add_library(deskgap_platform_winrt_preload_script STATIC
    ${CMAKE_CURRENT_BINARY_DIR}/dg_preload_winrt.c
)

target_link_libraries(deskgap_winrt PRIVATE deskgap_platform_winrt_preload_script)

bin2code(${CMAKE_CURRENT_LIST_DIR}/dg_preload_trident.js ${CMAKE_CURRENT_BINARY_DIR}/dg_preload_trident.c)
bin2code(${CMAKE_CURRENT_LIST_DIR}/dg_preload_winrt.js ${CMAKE_CURRENT_BINARY_DIR}/dg_preload_winrt.c)

target_sources(deskgap_platform PRIVATE
    argv.cpp
    app.cpp
    dialog.cpp
    menu.cpp
    dispatch.cpp
    shell.cpp
    system_preferences.cpp
    trident_webview.cpp
    webview.cpp
    exception.cpp
    BrowserWindow.cpp
    delay_load_hook.cpp
    util/dpi.cpp
    util/winrt.cpp
)

target_link_libraries(deskgap_platform PRIVATE deskgap_platform_trident_preload_script deskgap_winrt)
target_link_options(deskgap_platform PUBLIC "/DELAYLOAD:deskgap_winrt.dll")
