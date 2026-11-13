target_sources(crucible_gui
    PRIVATE
        gui_state.cpp
        main.cpp
        main_window.cpp
)

add_subdirectory(logging)
add_subdirectory(module_inspector)
add_subdirectory(op_graph)
add_subdirectory(preferences)
add_subdirectory(ram)
add_subdirectory(util)
