target_sources(crucible_gui
    PRIVATE
        main_window.hpp
        stylesheet.hpp
)

add_subdirectory(log_panel)
add_subdirectory(op_graph)
add_subdirectory(preferences)
add_subdirectory(util)
