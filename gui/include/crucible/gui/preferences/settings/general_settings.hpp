#pragma once

#include <QObject>

class QSettings;

namespace crucible::gui
{

struct GeneralSettings
    : public QObject
{
    Q_OBJECT;

public:
    enum Property
    {
    };

    explicit GeneralSettings();

    void load(QSettings& settings);
    void save(QSettings& settings) const;
    void resetToDefaults();

signals:
    void modified(Property property);
};

} // crucible::gui
