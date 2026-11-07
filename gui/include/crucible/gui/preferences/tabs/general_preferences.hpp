#pragma once

#include <QWidget>

class QSettings;

namespace crucible::gui
{

class GeneralPreferences
    : public QWidget
{
    Q_OBJECT;

public:
    explicit GeneralPreferences(QWidget* parent = nullptr);

signals:
    void modified();

public:
    void save(QSettings& settings) const;
    void resetToDefaults();
};

} // crucible::gui
