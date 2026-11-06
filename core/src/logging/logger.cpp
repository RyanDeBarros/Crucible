#include <crucible/core/logging/logger.hpp>

namespace crucible
{

core::Logger LOG;

} // crucible

namespace crucible::core
{

LogLine::LogLine(Logger& logger, LogLevel level, LogCategory category)
    : logger_(logger)
    , level_(level)
    , category_(std::move(category))
    , lock_(logger.acquire_lock())
{
}

LogLine::~LogLine()
{
    try
    {
        logger_.submit(level_, std::move(category_), str());
    }
    catch (...)
    {
    }
}

Logger::LockedEntries::LockedEntries(Logger& logger)
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

LogStore& Logger::LockedEntries::operator*()
{
    return logger().store_;
}

const LogStore* Logger::LockedEntries::operator->() const
{
    return &logger().store_;
}

LogStore* Logger::LockedEntries::operator->()
{
    return &logger().store_;
}

const Logger& Logger::LockedEntries::logger() const
{
    if (!logger_)
        throw std::runtime_error("Locked entries points to null logger");

    return *logger_;
}

Logger& Logger::LockedEntries::logger()
{
    if (!logger_)
        throw std::runtime_error("Locked entries points to null logger");

    return *logger_;
}

Logger::LockedEntries Logger::lock_entries()
{
    return LockedEntries(*this);
}

LogLine Logger::debug(LogCategory category)
{
    return LogLine(*this, LogLevel::Debug, std::move(category));
}

LogLine Logger::info(LogCategory category)
{
    return LogLine(*this, LogLevel::Info, std::move(category));
}

LogLine Logger::warning(LogCategory category)
{
    return LogLine(*this, LogLevel::Warning, std::move(category));
}

LogLine Logger::error(LogCategory category)
{
    return LogLine(*this, LogLevel::Error, std::move(category));
}

void Logger::submit(LogLevel level, LogCategory category, std::string message)
{
    LogEntry entry{
        .timestamp = std::chrono::system_clock::now(),
        .level = level,
        .category = std::move(category),
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
