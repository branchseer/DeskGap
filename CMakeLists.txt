cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)

project(DeskGap)
add_executable(DeskGap src/main.cc)

if(APPLE)
    add_library(DeskGapPlatform STATIC src/platform_mac.mm)

    file(GLOB nodeStaticLibFiles ${PROJECT_SOURCE_DIR}/deps/node/out/Release/*.a)
endif()

if(WIN32)
    add_library(DeskGapPlatform SHARED src/platform_win.cpp)
    target_link_libraries(DeskGapPlatform shcore)
    set_property(TARGET DeskGapPlatform PROPERTY VS_DOTNET_REFERENCES
        "System" "System.Windows.Forms"
    )
    set_target_properties(DeskGapPlatform PROPERTIES
        VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.7"
        COMMON_LANGUAGE_RUNTIME ""
    )
    
    target_link_libraries(DeskGap ws2_32 dbghelp crypt32 winmm iphlpapi psapi userenv)
    file(GLOB nodeStaticLibFiles ${PROJECT_SOURCE_DIR}/deps/node/Release/lib/*.lib)
endif()

target_link_libraries(DeskGap DeskGapPlatform)

list(FILTER nodeStaticLibFiles EXCLUDE REGEX "gtest")
list(FILTER nodeStaticLibFiles EXCLUDE REGEX "v8_nosnapshot")
target_link_libraries(DeskGap ${nodeStaticLibFiles})

if(APPLE)
    set_target_properties(
        DeskGap PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/src/resources/mac/Info.plist"
    )

    target_link_libraries(DeskGap "-framework Cocoa")

    add_custom_command(
        TARGET DeskGap POST_BUILD
        COMMAND
            $<$<CONFIG:Release>:strip>
            $<$<CONFIG:Release>:-x>
            $<$<CONFIG:Release>:$<TARGET_BUNDLE_CONTENT_DIR:DeskGap>/MacOS/DeskGap>
    )

endif()

set(DESKGAP_RESOURCE_FOLDER $<TARGET_FILE_DIR:DeskGap>/resources)
if(APPLE)
    set(DESKGAP_RESOURCE_FOLDER $<TARGET_BUNDLE_CONTENT_DIR:DeskGap>/Resources)
endif()


if (WIN32)
    target_sources(
        DeskGap PRIVATE
        "${CMAKE_SOURCE_DIR}/src/resources/win/App.manifest"
    )

    set_target_properties(
        DeskGap PROPERTIES
        WIN32_EXECUTABLE TRUE
        LINK_FLAGS "/ENTRY:\"mainCRTStartup\""
        COMPILE_OPTIONS "/MT"
    )

    add_custom_target(
        WebViewDll ALL
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/deps/Microsoft.Toolkit.Forms.UI.Controls.WebView.dll $<TARGET_FILE_DIR:DeskGap>
    )
    add_dependencies(WebViewDll DeskGap)
endif()


SET(DESKGAP_LIB_SRC_DIR ${CMAKE_SOURCE_DIR}/core)
SET(DESKGAP_LIB_DST_DIR ${DESKGAP_RESOURCE_FOLDER}/node_modules/deskgap)

if (WIN32)
    SET(DESKGAP_PRELOAD_SCRIPT preload_win.js)
elseif(APPLE)
    SET(DESKGAP_PRELOAD_SCRIPT preload_mac.js)
endif()

add_custom_target(
    DeskGapLib ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DESKGAP_LIB_DST_DIR}/build/Release/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DESKGAP_LIB_SRC_DIR}/build/Release/deskgap_native.node ${DESKGAP_LIB_DST_DIR}/build/Release/
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${DESKGAP_LIB_SRC_DIR}/dist/ ${DESKGAP_LIB_DST_DIR}/dist/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DESKGAP_LIB_SRC_DIR}/lib/ui/${DESKGAP_PRELOAD_SCRIPT} ${DESKGAP_LIB_DST_DIR}/dist/ui/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DESKGAP_LIB_SRC_DIR}/package_dist.json ${DESKGAP_LIB_DST_DIR}/package.json
)
add_dependencies(DeskGapLib DeskGap)

add_custom_target(
    DeskGapDefaultApp ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/app ${DESKGAP_RESOURCE_FOLDER}/app
)

add_dependencies(DeskGapDefaultApp DeskGap)
