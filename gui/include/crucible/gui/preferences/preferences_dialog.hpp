#pragma once

#include <QDialog>

class QDialogButtonBox;
class QSettings;
class QTabWidget;

namespace crucible::gui
{

class GeneralPreferences;
class LoggingPreferences;

class PreferencesDialog
    : public QDialog
{
    Q_OBJECT;

private:
    QTabWidget* tabs_ = nullptr;
    QDialogButtonBox* buttons_ = nullptr;

    GeneralPreferences* general_tab_ = nullptr;
    LoggingPreferences* logging_tab_ = nullptr;

public:
    explicit PreferencesDialog(QWidget* parent = nullptr);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void createLayout();

    void modified();
    void loadSettings();
    void saveSettings();
    void resetToDefaults();

    QSettings settings() const;
};

} // crucible::gui
