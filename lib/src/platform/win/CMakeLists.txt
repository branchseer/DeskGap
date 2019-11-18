target_compile_options(deskgap_platform
    PRIVATE -GR -await
)

target_compile_definitions(deskgap_platform PRIVATE WIN32_LEAN_AND_MEAN UNICODE _UNICODE DESKGAP_EXPORTING)

add_library(deskgap_platform_preload_scripts OBJECT
    ${CMAKE_CURRENT_BINARY_DIR}/dg_preload_trident.c
    ${CMAKE_CURRENT_BINARY_DIR}/dg_preload_winrt.c
)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dg_preload_trident.c ${CMAKE_CURRENT_BINARY_DIR}/dg_preload_winrt.c
    COMMAND node ${CMAKE_SOURCE_DIR}/scripts/bin2code.js ${CMAKE_CURRENT_LIST_DIR}/dg_preload_trident.js ${CMAKE_CURRENT_BINARY_DIR}/dg_preload_trident.c
    COMMAND node ${CMAKE_SOURCE_DIR}/scripts/bin2code.js ${CMAKE_CURRENT_LIST_DIR}/dg_preload_winrt.js ${CMAKE_CURRENT_BINARY_DIR}/dg_preload_winrt.c
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/dg_preload_trident.js ${CMAKE_CURRENT_LIST_DIR}/dg_preload_winrt.js
)

target_sources(deskgap_platform PRIVATE
    app.cpp
    dialog.cpp
    menu.cpp
)

target_link_libraries(deskgap_platform PRIVATE deskgap_platform_preload_scripts)
