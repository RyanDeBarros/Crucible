#pragma once

#include <crucible/core/logger.hpp>

#include <QAbstractTableModel>
#include <QDateTime>
#include <QString>

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
    core::Logger& logger_;
    std::unique_ptr<core::LogStoreFilter> filter_;

public:
    explicit LogModel(core::Logger& logger, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = {}) const override;
    int columnCount(const QModelIndex& parent = {}) const override;

    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void refresh();
    void setFilter(core::LogLevelMask mask);
    void clear();
};

} // crucible::gui
