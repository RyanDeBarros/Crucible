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

    void load(QSettings& settings);
    void save(QSettings& settings) const;
};

} // crucible::gui
