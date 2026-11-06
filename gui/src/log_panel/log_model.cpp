#include <crucible/gui/log_panel/log_model.hpp>

#include <QBrush>
#include <QDateTime>

namespace crucible::gui
{

LogModel::LogModel(core::Logger& logger, QObject* parent)
    : QAbstractTableModel(parent)
    , logger_(logger)
{
    setFilter(~0);
}

int LogModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(filter_->size());
}

int LogModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : Column::_Count;
}

QVariant LogModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(filter_->size()))
        return {};

    // lock logger for [] query
    auto entries = logger_.lock_entries();
    const auto& entry = (*filter_)[index.row()];

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case Column::Timestamp:
        {
            const auto timestamp = std::chrono::system_clock::to_time_t(entry.timestamp);
            return QDateTime::fromSecsSinceEpoch(timestamp).toString("HH:mm:ss.zzz");
        }

        case Column::Level:
            switch (entry.level)
            {
            case core::LogLevel::Debug:
                return "DEBUG";

            case core::LogLevel::Info:
                return "INFO";

            case core::LogLevel::Warning:
                return "WARNING";

            case core::LogLevel::Error:
                return "ERROR";

            default:
                return {};
            }

        case Column::Message:
            return QString::fromStdString(entry.message);
        }
    }

    if (role == Qt::ForegroundRole)
    {
        switch (index.column())
        {
        case Column::Timestamp:
            return QBrush(QColor(130, 130, 130)); // TODO use stylesheet file?

        case Column::Level:
        {
            switch (entry.level)
            {
            case core::LogLevel::Debug:
                return QBrush(QColor(130, 160, 190));

            case core::LogLevel::Info:
                return QBrush(QColor(130, 180, 130));

            case core::LogLevel::Warning:
                return QBrush(QColor(220, 170, 70));

            case core::LogLevel::Error:
                return QBrush(QColor(220, 90, 90));

            default:
                return {};
            }
        }

        default:
            return {};
        }
    }
    
    if (message_tooltip && role == Qt::ToolTipRole && index.column() == Column::Message)
        return QString::fromStdString(entry.message);

    return {};
}

QVariant LogModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};

    switch (section)
    {
    case Column::Timestamp:
        return "Time";

    case Column::Level:
        return "Level";

    case Column::Message:
        return "Message";

    default:
        return {};
    }
}

void LogModel::refresh()
{
    // lock logger for refresh
    auto entries = logger_.lock_entries();

    beginResetModel();
    filter_->refresh();
    endResetModel();
}

void LogModel::setFilter(core::LogLevelMask mask)
{
    auto entries = logger_.lock_entries();
    if (filter_ && filter_->mask() == mask)
        return;

    beginResetModel();
    filter_ = entries->filter(mask);
    endResetModel();
}

void LogModel::clear()
{
    auto entries = logger_.lock_entries();

    beginResetModel();
    entries->clear();
    filter_->refresh();
    endResetModel();
}

} // crucible::gui
