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

private:
    bool word_wrap_;
    int refresh_interval_;

public:
    explicit LoggingSettings();

    void load(QSettings& settings);
    void save(QSettings& settings) const;
    void resetToDefaults();

    bool wordWrap() const;
    int refreshInterval() const;

    void setWordWrap(bool word_wrap);
    void setRefreshInterval(int refresh_interval);

signals:
    void modified(Property property);
};

} // crucible::gui
