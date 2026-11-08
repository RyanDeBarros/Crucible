target_sources(crucible_gui
    PRIVATE
        gui_state.cpp
        main.cpp
        main_window.cpp
)

add_subdirectory(log_panel)
add_subdirectory(op_graph)
add_subdirectory(preferences)
add_subdirectory(util)
