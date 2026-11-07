#pragma once

#include <QWidget>

class QCheckBox;
class QSpinBox;
class QSettings;

namespace crucible::gui
{

class LoggingPreferences
    : public QWidget
{
    Q_OBJECT;

private:
    QCheckBox* word_wrap_ = nullptr;
    QSpinBox* refresh_interval_ = nullptr;

signals:
    void modified();

public:
    explicit LoggingPreferences(QWidget* parent = nullptr);

    void save(QSettings& settings) const;
    void resetToDefaults();
};

} // crucible::gui
