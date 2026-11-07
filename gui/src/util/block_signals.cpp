#include <crucible/gui/util/block_signals.hpp>

#include <QObject>

namespace crucible::gui
{

BlockSignals::BlockSignals(QObject& object)
    : object_(object)
    , was_blocked_(object.signalsBlocked())
{
    if (!was_blocked_)
        object_.blockSignals(true);
}

BlockSignals::~BlockSignals()
{
    if (!was_blocked_)
        object_.blockSignals(false);
}

BlockSignals::operator bool() const
{
    return true;
}

} // crucible::gui
