#pragma once

#include <crucible/core/logging/logger.hpp>

#include <QAbstractTableModel>

namespace crucible::gui
{

class LogModelRefreshCallback;

class LogModel
    : public QAbstractTableModel
{
    Q_OBJECT;

    friend class LogModelRefreshCallback;

public:
    enum Column
    {
        Timestamp,
        Level,
        Category,
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
