add_library(deskgap_platform STATIC)
set_target_properties(deskgap_platform PROPERTIES
    CXX_STANDARD 17
)

target_include_directories(deskgap_platform PRIVATE ../include/deskgap)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    add_subdirectory(mac)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    add_subdirectory(win)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    add_subdirectory(gtk)
endif()
