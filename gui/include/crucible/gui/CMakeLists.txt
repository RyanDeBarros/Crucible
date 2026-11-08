target_sources(crucible_gui
    PRIVATE
        gui_state.hpp
        main_window.hpp
)

add_subdirectory(log_panel)
add_subdirectory(op_graph)
add_subdirectory(preferences)
add_subdirectory(util)
