#pragma once

#include <QObject>

class QSettings;

namespace crucible::gui
{

struct LoggingSettings
    : public QObject
{
    Q_OBJECT;

public:
    enum Property
    {
        WordWrap,
        RefreshInterval,
    };

    bool word_wrap;
    int refresh_interval;

    explicit LoggingSettings();

    void load(QSettings& settings);
    void save(QSettings& settings) const;
    void resetToDefaults();

signals:
    void modified(Property property);
};

} // crucible::gui
