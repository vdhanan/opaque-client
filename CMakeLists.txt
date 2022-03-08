if (POLICY CMP0048)
cmake_policy(SET CMP0048 NEW)
endif ()

project(opaque_client)
cmake_minimum_required(VERSION 3.15)

set (CMAKE_CXX_STANDARD 17)

option(ENABLE_LTO "Enable cross language linking time optimization" ON)
if(ENABLE_LTO)
include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)
if(supported)
    message(STATUS "IPO / LTO enabled")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    add_link_options(-fuse-ld=lld)
else()
    message(STATUS "IPO / LTO not supported: <${error}>")
endif()
endif()

set(CURL_LIBRARY "-lcurl")

include_directories(${CMAKE_BINARY_DIR}/opaque-ke-cxx)

add_subdirectory(opaque-ke-cxx)

add_executable(main main.cpp)

target_link_libraries(main opaque_ke_cxx ${CURL_LIBRARY})