find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    OpenGL
)

include(FetchContent)

qt_standard_project_setup()

set(QT_NODES_DEVELOPER_DEFAULTS OFF)
set(USE_QT6 ON)
set(BUILD_SHARED_LIBS ON)
set(BUILD_TESTING OFF)
set(BUILD_EXAMPLES OFF)
set(BUILD_DOCS OFF)

FetchContent_Declare(
    QtNodes
    GIT_REPOSITORY https://github.com/paceholder/nodeeditor.git
    GIT_TAG        7bbcd3e08c8330e9a5b47dbb730955849b8f547c
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(QtNodes)

qt_add_executable(crucible_gui)

set_target_properties(crucible_gui PROPERTIES
    AUTORCC ON
)

target_compile_features(crucible_gui
    PRIVATE
        cxx_std_23
)

target_link_libraries(crucible_gui
    PRIVATE
        crucible::core
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::OpenGL
        QtNodes::QtNodes
)

add_custom_command(TARGET crucible_gui POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:QtNodes>"
        "$<TARGET_FILE_DIR:crucible_gui>"
    COMMENT "Copying QtNodes runtime DLL"
)

target_include_directories(crucible_gui
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(crucible_gui
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/crucible.qrc
)

add_subdirectory(include/crucible/gui)
add_subdirectory(src)
