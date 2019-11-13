cmake_minimum_required(VERSION 3.13)

project(deskgap)

add_library(deskgap STATIC)
target_include_directories(deskgap PUBLIC src/include)
set_target_properties(deskgap PROPERTIES
    CXX_STANDARD 17
)

add_subdirectory(src/platform)

target_link_libraries(deskgap deskgap_platform)

target_sources(deskgap PRIVATE
    src/wrappers/app_c.cc
)
