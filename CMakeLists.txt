cmake_minimum_required(VERSION 3.13)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.10)
include(FetchContent)
if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

project(DeskGap)

add_subdirectory(lib)

FetchContent_Declare(
    nod
    GIT_REPOSITORY http://github.com/patr0nus/nod
)
FetchContent_MakeAvailable(nod)

add_executable(NodeDeskGap src/main.cc)
set_target_properties(
    NodeDeskGap PROPERTIES
    OUTPUT_NAME DeskGap
    CXX_STANDARD 17
)
target_link_libraries(NodeDeskGap deskgap nod)

if(APPLE)
    set_target_properties(
        NodeDeskGap PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/src/resources/mac/Info.plist
    )
endif()

if (NOT APPLE)
    set_target_properties(NodeDeskGap
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY $<CONFIG>/DeskGap
    )
endif()

if (APPLE OR LINUX)
    set(DeskGapExecutable $<TARGET_FILE:NodeDeskGap>)
    if (APPLE)
        set(DeskGapExecutable $<TARGET_BUNDLE_CONTENT_DIR:NodeDeskGap>/MacOS/DeskGap)
    endif()
    add_custom_command(
        TARGET NodeDeskGap POST_BUILD
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

