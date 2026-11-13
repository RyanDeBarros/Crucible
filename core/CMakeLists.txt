add_library(crucible_core STATIC)
add_library(crucible::core ALIAS crucible_core)

target_include_directories(crucible_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(crucible_core
    PUBLIC
        cxx_std_23
)

add_subdirectory(include/external)
add_subdirectory(include/crucible/core)
add_subdirectory(src)
