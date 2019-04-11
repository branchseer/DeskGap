cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

project(DeskGap)
add_executable(DeskGap src/main.cc)

if (NOT APPLE)
    set_target_properties(DeskGap
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY $<CONFIG>/DeskGap
    )
endif()

file(GLOB nodeStaticLib ${PROJECT_SOURCE_DIR}/deps/node/release/*)

if (APPLE)
    add_library(DeskGapPlatform STATIC src/platform_mac.mm)
elseif (WIN32)
    add_library(DeskGapPlatform STATIC src/platform_win.cpp)
    set_target_properties(
        DeskGapPlatform PROPERTIES
        COMPILE_OPTIONS "/MT"
        COMPILE_DEFINITIONS WIN32_LEAN_AND_MEAN=1
    )
    target_link_libraries(DeskGapPlatform shcore)
    target_link_libraries(DeskGap ws2_32 dbghelp crypt32 winmm iphlpapi psapi userenv)
elseif (LINUX)
    FIND_PACKAGE(PkgConfig REQUIRED)
    PKG_CHECK_MODULES(GTK REQUIRED gtk+-3.0)

    add_library(DeskGapPlatform STATIC src/platform_gtk.cpp)
    target_include_directories(DeskGapPlatform PRIVATE ${GTK_INCLUDE_DIRS})
    target_link_libraries(DeskGapPlatform stdc++fs ${GTK_LIBRARIES} ${GTK_CFLAGS_OTHER})

    list(INSERT nodeStaticLib 0 "-Wl,--start-group")
    list(APPEND nodeStaticLib "-Wl,--end-group" "-pthread" "-ldl" "-lrt" "-lm" "-rdynamic")
endif()

target_link_libraries(DeskGap DeskGapPlatform)
target_link_libraries(DeskGap ${nodeStaticLib})

if(APPLE)
    set_target_properties(
        DeskGap PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/src/resources/mac/Info.plist"
    )

    target_link_libraries(DeskGap "-framework Cocoa")
endif()

if (APPLE OR LINUX)
    set(DeskGapExecutable $<TARGET_FILE:DeskGap>)
    if (APPLE)
        set(DeskGapExecutable $<TARGET_BUNDLE_CONTENT_DIR:DeskGap>/MacOS/DeskGap)
    endif()
    add_custom_command(
        TARGET DeskGap POST_BUILD
        COMMAND
            $<$<CONFIG:Release>:strip>
            $<$<CONFIG:Release>:-x>
            $<$<CONFIG:Release>:${DeskGapExecutable}>
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
        LINK_FLAGS "/ENTRY:\"wmainCRTStartup\""
        COMPILE_OPTIONS "/MT"
        COMPILE_DEFINITIONS WIN32_LEAN_AND_MEAN=1
    )
endif()


SET(DESKGAP_LIB_SRC_DIR ${CMAKE_SOURCE_DIR}/core)
SET(DESKGAP_LIB_DST_DIR ${DESKGAP_RESOURCE_FOLDER}/node_modules/deskgap)

if (WIN32)
    SET(DESKGAP_PRELOAD_SCRIPTS
        ${DESKGAP_LIB_SRC_DIR}/lib/ui/preload_winrt.js
        ${DESKGAP_LIB_SRC_DIR}/lib/ui/preload_trident.js
        ${CMAKE_SOURCE_DIR}/core/node_modules/es6-promise/dist/es6-promise.auto.min.js
    )
    SET(COPY_POLYFILL_COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/core/node_modules/es6-promise/dist/es6-promise.auto.min.js ${DESKGAP_LIB_DST_DIR}/dist/ui/)
elseif(APPLE)
    SET(DESKGAP_PRELOAD_SCRIPTS ${DESKGAP_LIB_SRC_DIR}/lib/ui/preload_mac.js)
elseif (LINUX)
    SET(DESKGAP_PRELOAD_SCRIPTS ${DESKGAP_LIB_SRC_DIR}/lib/ui/preload_gtk.js)
endif()

if (WIN32)
    SET(DESKGAP_CORE_NATIVE_FILES
        ${DESKGAP_LIB_SRC_DIR}/build/Release/deskgap_native.node
        ${DESKGAP_LIB_SRC_DIR}/build/Release/deskgap_winrt.dll
    )
else()
    SET(DESKGAP_CORE_NATIVE_FILES
        ${DESKGAP_LIB_SRC_DIR}/build/Release/deskgap_native.node
    )
endif()

add_custom_target(
    DeskGapLib ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DESKGAP_LIB_DST_DIR}/build/Release/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DESKGAP_CORE_NATIVE_FILES} ${DESKGAP_LIB_DST_DIR}/build/Release/
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${DESKGAP_LIB_SRC_DIR}/dist/ ${DESKGAP_LIB_DST_DIR}/dist/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DESKGAP_PRELOAD_SCRIPTS} ${DESKGAP_LIB_DST_DIR}/dist/ui/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DESKGAP_LIB_SRC_DIR}/package_dist.json ${DESKGAP_LIB_DST_DIR}/package.json
    COMMAND ${COPY_POLYFILL_COMMAND}
)
add_dependencies(DeskGapLib DeskGap)

add_custom_target(
    DeskGapDefaultApp ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/app ${DESKGAP_RESOURCE_FOLDER}/app
)

add_dependencies(DeskGapDefaultApp DeskGap)
