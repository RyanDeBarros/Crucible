#pragma once

#include <QAbstractTableModel>
#include <QDateTime>
#include <QString>

namespace crucible::core
{

class Logger;

} // crucible::core

namespace crucible::gui
{

class LogModel
    : public QAbstractTableModel
{
    Q_OBJECT;

public:
    enum Column
    {
        Timestamp,
        Level,
        Message,
        _Count
    };

public:
    bool message_tooltip = false;

private:
    const core::Logger& logger_;

public:
    explicit LogModel(const core::Logger& logger, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = {}) const override;
    int columnCount(const QModelIndex& parent = {}) const override;

    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void refresh();
};

} // crucible::gui
