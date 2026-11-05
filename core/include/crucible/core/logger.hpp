#pragma once

#include <chrono>
#include <mutex>
#include <sstream>
#include <vector>

namespace crucible::core
{

enum class LogLevel
{
    Debug,
    Info,
    Warning,
    Error
};

struct LogEntry
{
    std::chrono::system_clock::time_point timestamp;
    LogLevel level;
    std::string message;
};

class Logger;

class LogLine
    : public std::ostringstream
{
    friend class Logger;

private:
    Logger& logger_;
    LogLevel level_;
    std::unique_lock<std::mutex> lock_;

private:
    LogLine(Logger& logger, LogLevel level);

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
        const Logger* logger_;
        std::unique_lock<std::mutex> lock_;

    private:
        LockedEntries(const Logger& logger);

    public:
        LockedEntries(const LockedEntries&) = delete;
        LockedEntries(LockedEntries&&) noexcept;
        
        LockedEntries& operator=(const LockedEntries&) = delete;
        LockedEntries& operator=(LockedEntries&&) = delete;

        size_t size() const;
        bool empty() const;
        const LogEntry& operator[](size_t index) const;
        auto begin() const;
        auto end() const;
        auto cbegin() const;
        auto cend() const;

    private:
        const Logger& logger() const;
    };

private:
    std::vector<LogEntry> entries_;
    mutable std::mutex mutex_;

public:
    LockedEntries lock_entries() const;

    LogLine debug();
    LogLine info();
    LogLine warning();
    LogLine error();

private:
    void submit(LogLevel level, std::string message);
    std::unique_lock<std::mutex> acquire_lock() const;
};

extern Logger LOG;

} // crucible::core
