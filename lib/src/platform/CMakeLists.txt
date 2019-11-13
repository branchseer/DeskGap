add_library(deskgap_platform STATIC)
target_link_libraries(deskgap_platform PUBLIC "-framework AppKit" "-framework WebKit")
set_target_properties(deskgap_platform PROPERTIES
    CXX_STANDARD 17
)

target_include_directories(deskgap_platform PRIVATE ../include/deskgap)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    add_subdirectory(mac)
endif()
