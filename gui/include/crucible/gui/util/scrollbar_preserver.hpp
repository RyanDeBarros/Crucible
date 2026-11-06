#pragma once

class QAbstractItemView;

namespace crucible::gui
{

class ScrollbarPreserver
{
    QAbstractItemView& view_;
    bool at_bottom_ = false;

public:
    ScrollbarPreserver(QAbstractItemView& view);
    ScrollbarPreserver(const ScrollbarPreserver&) = delete;
    ScrollbarPreserver(ScrollbarPreserver&&) = delete;
    ~ScrollbarPreserver();

    ScrollbarPreserver& operator=(const ScrollbarPreserver&) = delete;
    ScrollbarPreserver& operator=(ScrollbarPreserver&&) = delete;
};

} // crucible::gui
