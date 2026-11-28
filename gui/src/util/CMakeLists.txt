target_sources(crucible_gui
    PRIVATE
        alerts.cpp
        dialog_underlay.cpp
        fuzzy.cpp
        icons.cpp
        item_delegates.cpp
        layout_helpers.cpp
        modal_dialog.cpp
        outlines.cpp
        repr.cpp
        scrollbar_preserver.cpp
        standard_tree.cpp
        stylesheet.cpp
)

add_subdirectory(type_selector)
