cmake_minimum_required(VERSION 3.13)

project(deskgap)

add_library(deskgap STATIC src/index.cc)
set_target_properties(deskgap PROPERTIES
    CXX_STANDARD 17
)

if(${CMAKE_SYSTEM_NAME} STREQUAL Darwin)
    add_subdirectory(src/mac)
endif()

target_link_libraries(deskgap deskgap_platform)

#target_sources(DeskGapCore
#
#)
