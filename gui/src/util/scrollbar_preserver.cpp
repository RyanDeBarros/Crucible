#include <crucible/gui/util/scrollbar_preserver.hpp>

#include <QAbstractItemView>
#include <QScrollBar>

namespace crucible::gui
{

ScrollbarPreserver::ScrollbarPreserver(QAbstractItemView& view)
    : view_(view)
{
    auto* scrollbar = view_.verticalScrollBar();
    at_bottom_ = scrollbar->value() >= scrollbar->maximum();
}

ScrollbarPreserver::~ScrollbarPreserver()
{
    if (at_bottom_)
        view_.scrollToBottom();
}

} // crucible::gui
