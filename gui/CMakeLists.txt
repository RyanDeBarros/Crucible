find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    OpenGL
)

include(FetchContent)

qt_standard_project_setup()

set(QT_NODES_DEVELOPER_DEFAULTS OFF)

FetchContent_Declare(
    QtNodes
    GIT_REPOSITORY https://github.com/paceholder/nodeeditor.git
    GIT_TAG        v3  # TODO use commit hash
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(QtNodes)

qt_add_executable(crucible_gui)

target_compile_features(crucible_gui
    PRIVATE
        cxx_std_23
)

target_include_directories(crucible_gui
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(crucible_gui
    PRIVATE
        crucible::core
        Qt6::Widgets
        QtNodes::QtNodes
)

add_subdirectory(src)
