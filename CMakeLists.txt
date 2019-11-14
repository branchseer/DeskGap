cmake_minimum_required(VERSION 3.13)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.10)
include(FetchContent)
if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

project(DeskGap)

add_subdirectory(lib)
add_subdirectory(js)

FetchContent_Declare(
    nod
    GIT_REPOSITORY http://github.com/patr0nus/nod
)
FetchContent_MakeAvailable(nod)

add_executable(DeskGapNode
    src/main.cc
    src/node_bindings/index.cc
    src/dispatch/node_dispatch.cc
    src/dispatch/ui_dispatch.cc
)
set_target_properties(
    DeskGapNode PROPERTIES
    OUTPUT_NAME DeskGap
    CXX_STANDARD 17
)
target_link_libraries(DeskGapNode deskgap nod DeskGapNodeScript)
target_include_directories(DeskGapNode PRIVATE src/node_api js/node_modules/node-addon-api)


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

set(DESKGAP_RESOURCE_FOLDER $<TARGET_FILE_DIR:DeskGap>/resources)
if(APPLE)
    set(DESKGAP_RESOURCE_FOLDER $<TARGET_BUNDLE_CONTENT_DIR:DeskGap>/Resources)
endif()

if (WIN32)
    target_sources(
        DeskGap PRIVATE
        ${CMAKE_SOURCE_DIR}/src/resources/win/App.manifest
    )

    set_target_properties(
        DeskGap PROPERTIES
        WIN32_EXECUTABLE TRUE
        LINK_FLAGS "/ENTRY:\"wmainCRTStartup\""
        COMPILE_OPTIONS "/MT"
        COMPILE_DEFINITIONS WIN32_LEAN_AND_MEAN=1
    )
endif()
