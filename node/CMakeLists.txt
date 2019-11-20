cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)

if(WIN32)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.10)
endif()

include(FetchContent)
if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

project(DeskGap)

set(DG_NODE_MODULES_DIR ${CMAKE_CURRENT_LIST_DIR}/../node_modules)
set(DG_NODE_BIN_DIR ${DG_NODE_MODULES_DIR}/.bin)
set(DG_LIB_DIR ${CMAKE_CURRENT_LIST_DIR}/../lib)

add_subdirectory(${DG_LIB_DIR} lib)
set(DG_BIN2CODE node ${DG_LIB_DIR}/bin2code.js)
set(NPM_WEBPACK npm --prefix ${CMAKE_CURRENT_LIST_DIR}/.. run webpack --)
if(WIN32)
    set(NPM_WEBPACK cmd /c ${NPM_WEBPACK})
endif()
add_subdirectory(js)

FetchContent_Declare(
    nod
    GIT_REPOSITORY https://github.com/patr0nus/nod
)
FetchContent_MakeAvailable(nod)

add_executable(DeskGapNode
    src/main.cc
    src/node_bindings/index.cc
    src/node_bindings/dispatch/node_dispatch.cc
    src/node_bindings/dispatch/ui_dispatch.cc
    src/node_bindings/app/app_wrap.cc
    src/node_bindings/dialog/dialog_wrap.cc
    src/node_bindings/menu/menu_wrap.cc
    src/node_bindings/shell/shell_wrap.cc
    src/node_bindings/system_preferences/system_preferences_wrap.cc
    src/node_bindings/webview/webview_wrap.cc
    src/node_bindings/window/browser_window_wrap.cc
)
set_target_properties(
    DeskGapNode PROPERTIES
    OUTPUT_NAME DeskGap
    CXX_STANDARD 17
)
target_link_libraries(DeskGapNode deskgap nod DeskGapNodeScripts)
target_include_directories(DeskGapNode PRIVATE src/node_api ${DG_NODE_MODULES_DIR}/node-addon-api)

if(APPLE)
    set_target_properties(
        DeskGapNode PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/src/resources/mac/Info.plist
    )
endif()

if (NOT APPLE)
    set_target_properties(DeskGapNode
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY $<CONFIG>/DeskGap
    )
endif()

if (APPLE OR LINUX)
    add_custom_command(
        TARGET DeskGapNode POST_BUILD
        COMMAND
        $<$<CONFIG:Release>:strip>
        $<$<CONFIG:Release>:-x>
        $<$<CONFIG:Release>:$<TARGET_FILE:DeskGapNode>>
    )
endif()

if (WIN32)
    target_sources(
        DeskGapNode PRIVATE
        ${CMAKE_SOURCE_DIR}/src/resources/win/App.manifest
    )

    set_target_properties(
        DeskGapNode PROPERTIES
        WIN32_EXECUTABLE $<IF:$<CONFIG:Release>,TRUE,FALSE>
        LINK_FLAGS "/ENTRY:\"wmainCRTStartup\""
    )
endif()

set_target_properties(DeskGapNode PROPERTIES
    RESOURCE app
)

set(DESKGAP_RESOURCE_FOLDER $<TARGET_FILE_DIR:DeskGapNode>/resources)
if(APPLE)
    set(DESKGAP_RESOURCE_FOLDER $<TARGET_BUNDLE_CONTENT_DIR:DeskGapNode>/Resources)
endif()

add_custom_target(DeskGapNodeResources ALL ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/app ${DESKGAP_RESOURCE_FOLDER}/app)
add_dependencies(DeskGapNodeResources DeskGapNode)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    add_custom_target(DeskGapWinRTDLL ALL ${CMAKE_COMMAND} -E copy $<TARGET_FILE:deskgap_winrt> $<TARGET_FILE_DIR:DeskGapNode>)
endif()
