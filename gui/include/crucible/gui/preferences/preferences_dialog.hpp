#pragma once

#include <crucible/gui/util/modal_dialog.hpp>

class QDialogButtonBox;
class QSettings;
class QTabWidget;

namespace crucible::gui
{

class GeneralPreferences;
class LoggingPreferences;

class PreferencesDialog
    : public ModalDialog
{
    Q_OBJECT;

private:
    QTabWidget* tabs_ = nullptr;
    QDialogButtonBox* buttons_ = nullptr;

    GeneralPreferences* general_tab_ = nullptr;
    LoggingPreferences* logging_tab_ = nullptr;

public:
    explicit PreferencesDialog(QWidget* parent = nullptr);

private:
    void modified();
    void saveSettings();
    void resetToDefaults();
};

} // crucible::gui
