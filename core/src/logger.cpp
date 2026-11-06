#include <crucible/core/logger.hpp>

namespace crucible
{

core::Logger LOG;

} // crucible

namespace crucible::core
{

LogStoreFilter::LogStoreFilter(const LogStore& store, LogLevelMask mask)
    : store_(store)
    , mask_(mask)
    , store_version_(store.version_)
{
    append_missing();
}

LogLevelMask LogStoreFilter::mask() const
{
    return mask_;
}

size_t LogStoreFilter::size() const
{
    assert_valid();
    return chronology_.size();
}

const LogEntry& LogStoreFilter::operator[](size_t index) const
{
    assert_valid();
    const auto& ref = chronology_[index];
    return store_.entry_lists_[static_cast<size_t>(ref.level)][ref.index];
}

void LogStoreFilter::refresh()
{
    if (store_version_ != store_.version_)
    {
        chronology_.clear();
        end_index_ = 0;
        store_version_ = store_.version_;
    }

    append_missing();
}

void LogStoreFilter::append_missing()
{
    while (end_index_ < store_.chronology_.size())
    {
        LogEntryReference ref = store_.chronology_[end_index_++];
        if (mask_ & logLevelMask(ref.level))
            chronology_.push_back(ref);
    }
}

void LogStoreFilter::assert_valid() const
{
    if (store_version_ != store_.version_)
        throw std::runtime_error("Log store filter is not up to date: missing refresh() call");
}

void LogStore::add_entry(LogEntry entry)
{
    auto& list = entry_lists_[static_cast<size_t>(entry.level)];
    chronology_.emplace_back(entry.level, list.size());
    list.push_back(std::move(entry));
}

void LogStore::clear()
{
    chronology_.clear();
    for (auto& list : entry_lists_)
        list.clear();

    ++version_;
}

std::unique_ptr<LogStoreFilter> LogStore::filter(LogLevelMask mask) const
{
    return std::make_unique<LogStoreFilter>(*this, mask);
}

LogLine::LogLine(Logger& logger, LogLevel level)
    : logger_(logger)
    , level_(level)
    , lock_(logger.acquire_lock())
{
}

LogLine::~LogLine()
{
    try
    {
        logger_.submit(level_, str());
    }
    catch (...)
    {
    }
}

Logger::LockedEntries::LockedEntries(const Logger& logger)
    : logger_(&logger)
    , lock_(logger.acquire_lock())
{
}

Logger::LockedEntries::LockedEntries(LockedEntries&& o) noexcept
    : logger_(std::exchange(o.logger_, nullptr))
    , lock_(std::move(o.lock_))
{
}

const LogStore& Logger::LockedEntries::operator*() const
{
    return logger().store_;
}

const LogStore* Logger::LockedEntries::operator->() const
{
    return &logger().store_;
}

const Logger& Logger::LockedEntries::logger() const
{
    if (!logger_)
        throw std::runtime_error("Locked entries points to null logger");

    return *logger_;
}

Logger::LockedEntries Logger::lock_entries() const
{
    return LockedEntries(*this);
}

LogLine Logger::debug()
{
    return LogLine(*this, LogLevel::Debug);
}

LogLine Logger::info()
{
    return LogLine(*this, LogLevel::Info);
}

LogLine Logger::warning()
{
    return LogLine(*this, LogLevel::Warning);
}

LogLine Logger::error()
{
    return LogLine(*this, LogLevel::Error);
}

void Logger::submit(LogLevel level, std::string message)
{
    LogEntry entry{
        .timestamp = std::chrono::system_clock::now(),
        .level = level,
        .message = std::move(message)
    };

    store_.add_entry(std::move(entry));
}

std::unique_lock<std::mutex> Logger::acquire_lock() const
{
    std::unique_lock lock(mutex_, std::try_to_lock);

    if (!lock.owns_lock())
        throw std::runtime_error("Logger is already locked");

    return lock;
}

} // crucible::core
