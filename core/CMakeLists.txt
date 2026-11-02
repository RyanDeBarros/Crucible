add_library(crucible_core STATIC)
add_library(crucible::core ALIAS crucible_core)

target_include_directories(crucible_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/crucible/core
)

target_compile_features(crucible_core
    PUBLIC
        cxx_std_23
)

add_subdirectory(src)
