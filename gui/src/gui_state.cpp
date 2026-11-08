#include <crucible/gui/gui_state.hpp>

#include <QStandardPaths>

namespace crucible::gui
{

GuiState::GuiState()
    : QSettings(
        QStandardPaths::writableLocation(QStandardPaths::StateLocation) + "/gui_state.ini",
        QSettings::IniFormat)
    , logging(*this)
{
}

GuiState::Logging::Logging(GuiState& state)
    : state_(state)
{
}

GuiState::Value<bool> GuiState::Logging::word_wrap() const
{
    return Value<bool>(state_, "logging.word_wrap", false);
}

} // crucible::gui
