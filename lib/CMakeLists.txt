cmake_minimum_required(VERSION 3.13)

include(FetchContent)

project(deskgap)

add_library(deskgap STATIC)
target_include_directories(deskgap PUBLIC src/include)
set_target_properties(deskgap PROPERTIES
    CXX_STANDARD 17
)

FetchContent_Declare(
    bin2code
    GIT_REPOSITORY https://github.com/patr0nus/bin2code
    GIT_TAG 8499e274c959483c3ecfc10587567a700f083438
)
FetchContent_MakeAvailable(bin2code)

add_subdirectory(src/platform)

target_link_libraries(deskgap PUBLIC deskgap_platform)

target_sources(deskgap PRIVATE
    src/wrappers/app_c.cc
)
