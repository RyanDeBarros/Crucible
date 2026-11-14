#include <crucible/gui/util/repr.hpp>

#include <format>

namespace crucible::gui
{

QString repr(core::typing::Specialization spec)
{
    return QString::fromStdString(std::format("0x{:08X}", spec));
}

} // crucible::gui
