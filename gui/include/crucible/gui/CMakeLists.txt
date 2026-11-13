target_sources(crucible_gui
    PRIVATE
        gui_state.hpp
        main_window.hpp
)

add_subdirectory(logging)
add_subdirectory(module_inspector)
add_subdirectory(op_graph)
add_subdirectory(preferences)
add_subdirectory(ram)
add_subdirectory(util)
