#include <crucible/gui/log_panel/log_model.hpp>

#include <crucible/gui/log_panel/log_categories.hpp>
#include <crucible/gui/log_panel/log_column.hpp>

#include <QBrush>
#include <QDateTime>

namespace crucible::gui
{

class LogModelRefreshCallback
    : public core::LogRefreshCallback
{
private:
    LogModel& model_;

public:
    explicit LogModelRefreshCallback(LogModel& model);

    void before_append(std::size_t row) override;
    void after_append() override;
};

LogModelRefreshCallback::LogModelRefreshCallback(LogModel& model)
    : model_(model)
{
}

void LogModelRefreshCallback::before_append(std::size_t row)
{
    model_.beginInsertRows(QModelIndex(), static_cast<int>(row), static_cast<int>(row));
}

void LogModelRefreshCallback::after_append()
{
    model_.endInsertRows();
}

LogModel::LogModel(core::Logger& logger, QObject* parent)
    : QAbstractTableModel(parent)
    , logger_(logger)
{
    setLevelFilter(core::LogLevelMask::full());
}

int LogModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(filter_->size());
}

int LogModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : LogColumn::_Count;
}

// TODO filter for which columns besides 'message' should be shown

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
        case LogColumn::Timestamp:
        {
            const auto timestamp = std::chrono::system_clock::to_time_t(entry.timestamp);
            return QDateTime::fromSecsSinceEpoch(timestamp).toString("HH:mm:ss.zzz");
        }

        case LogColumn::Level:
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

        case LogColumn::Category:
            return QString::fromStdString(entry.category.text);

        case LogColumn::Message:
            return QString::fromStdString(entry.message);
        }
    }

    if (role == Qt::ForegroundRole)
    {
        switch (index.column())
        {
        case LogColumn::Timestamp:
            return QBrush(QColor(130, 130, 130)); // TODO use stylesheet file?

        case LogColumn::Level:
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

        case LogColumn::Category:
            return QBrush(logCategoryColor(entry.category));

        default:
            return {};
        }
    }
    
    if (message_tooltip && role == Qt::ToolTipRole && index.column() == LogColumn::Message)
        return QString::fromStdString(entry.message);

    return {};
}

QVariant LogModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};

    switch (section)
    {
    case LogColumn::Timestamp:
        return "Time";

    case LogColumn::Level:
        return "Level";

    case LogColumn::Category:
        return "Category";

    case LogColumn::Message:
        return "Message";

    default:
        return {};
    }
}

void LogModel::refresh()
{
    // lock logger for refresh
    auto entries = logger_.lock_entries();

    if (filter_->invalidated())
    {
        beginResetModel();
        filter_->refresh();
        endResetModel();
        return;
    }

    LogModelRefreshCallback callback(*this);
    filter_->refresh(&callback);
}

void LogModel::setLevelFilter(core::LogLevelMask mask)
{
    {
        auto entries = logger_.lock_entries();
        if (filter_ && filter_->mask() == mask)
            return;

        beginResetModel();
        filter_ = entries->filter(mask);
        endResetModel();
    }
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
