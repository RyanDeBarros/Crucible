#include <crucible/core/logger.hpp>

namespace crucible::core
{

extern Logger LOG;

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

size_t Logger::LockedEntries::size() const
{
    return logger().entries_.size();
}

bool Logger::LockedEntries::empty() const
{
    return logger().entries_.empty();
}

const LogEntry& Logger::LockedEntries::operator[](size_t index) const
{
    return logger().entries_[index];
}

auto Logger::LockedEntries::begin() const
{
    return logger().entries_.begin();
}

auto Logger::LockedEntries::end() const
{
    return logger().entries_.end();
}

auto Logger::LockedEntries::cbegin() const
{
    return logger().entries_.cbegin();
}

auto Logger::LockedEntries::cend() const
{
    return logger().entries_.cend();
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

    entries_.push_back(std::move(entry));
}

std::unique_lock<std::mutex> Logger::acquire_lock() const
{
    std::unique_lock lock(mutex_, std::try_to_lock);

    if (!lock.owns_lock())
        throw std::runtime_error("Logger is already locked");

    return lock;
}

} // crucible::core
