target_sources(crucible_gui
    PRIVATE
        main.cpp
        main_window.cpp
        stylesheet.cpp
)

add_subdirectory(log_panel)
add_subdirectory(op_graph)
add_subdirectory(preferences)
add_subdirectory(util)
