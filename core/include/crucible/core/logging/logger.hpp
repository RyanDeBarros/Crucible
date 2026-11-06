#pragma once

#include <crucible/core/logging/log_store.hpp>

#include <mutex>
#include <sstream>

namespace crucible::core
{

class Logger;

class LogLine
    : public std::ostringstream
{
    friend class Logger;

private:
    Logger& logger_;
    LogLevel level_;
    LogCategory category_;
    std::unique_lock<std::mutex> lock_;

private:
    LogLine(Logger& logger, LogLevel level, LogCategory category);

public:
    LogLine(const LogLine&) = delete;
    LogLine(LogLine&&) = delete;

    ~LogLine();

    LogLine& operator=(const LogLine&) = delete;
    LogLine& operator=(LogLine&&) = delete;
};

class Logger
{
    friend class LogLine;

public:
    class LockedEntries
    {
        friend class Logger;

    private:
        Logger* logger_;
        std::unique_lock<std::mutex> lock_;

    private:
        LockedEntries(Logger& logger);

    public:
        LockedEntries(const LockedEntries&) = delete;
        LockedEntries(LockedEntries&&) noexcept;
        
        LockedEntries& operator=(const LockedEntries&) = delete;
        LockedEntries& operator=(LockedEntries&&) = delete;

        const LogStore& operator*() const;
        LogStore& operator*();
        const LogStore* operator->() const;
        LogStore* operator->();

    private:
        const Logger& logger() const;
        Logger& logger();
    };

private:
    LogStore store_;
    mutable std::mutex mutex_;

public:
    LockedEntries lock_entries();

    LogLine debug(LogCategory category);
    LogLine info(LogCategory category);
    LogLine warning(LogCategory category);
    LogLine error(LogCategory category);

private:
    void submit(LogLevel level, LogCategory category, std::string message);
    std::unique_lock<std::mutex> acquire_lock() const;
};

} // crucible::core

namespace crucible
{

extern core::Logger LOG;

} // crucible
