if (CMAKE_BUILD_TYPE STREQUAL "Debug")
set(CARGO_CMD cargo build --verbose)
set(TARGET_DIR "debug")
else ()
set(CARGO_CMD cargo build --release --verbose)
set(TARGET_DIR "release")
endif ()

if(ENABLE_LTO)
set(RUST_FLAGS "-Clinker-plugin-lto" "-Clinker=clang-12" "-Clink-arg=-fuse-ld=lld-12")
endif()

set(OPAQUE_KE_CXX_LIB "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_DIR}/libopaque_ke_cxx.a")

set(OPAQUE_KE_CXX "${CMAKE_CURRENT_BINARY_DIR}/opaque_ke_cxx.cpp")
add_library(opaque_ke_cxx STATIC ${OPAQUE_KE_CXX})
add_custom_command(
OUTPUT ${OPAQUE_KE_CXX}
COMMAND CARGO_TARGET_DIR=${CMAKE_CURRENT_BINARY_DIR} RUSTFLAGS="${RUST_FLAGS}" ${CARGO_CMD}
COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/cxxbridge/opaque-ke-cxx/src/lib.rs.cc ${OPAQUE_KE_CXX}
COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/cxxbridge/opaque-ke-cxx/src/lib.rs.h ${CMAKE_CURRENT_BINARY_DIR}/opaque_ke_cxx.h
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(opaque_ke_cxx pthread dl ${OPAQUE_KE_CXX_LIB})

add_test(NAME opaque_ke_cxx_test 
COMMAND cargo test
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
