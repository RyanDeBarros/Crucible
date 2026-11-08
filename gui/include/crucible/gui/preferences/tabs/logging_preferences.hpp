#pragma once

#include <QWidget>

class QSpinBox;
class QSettings;

namespace crucible::gui
{

class LoggingPreferences
    : public QWidget
{
    Q_OBJECT;

private:
    QSpinBox* refresh_interval_ = nullptr;

public:
    explicit LoggingPreferences(QWidget* parent = nullptr);

    void save(QSettings& settings) const;
    void resetToDefaults();

signals:
    void modified();
};

} // crucible::gui
