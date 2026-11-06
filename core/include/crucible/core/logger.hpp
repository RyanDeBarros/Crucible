#pragma once

#include <array>
#include <chrono>
#include <mutex>
#include <sstream>
#include <vector>

namespace crucible::core
{

enum class LogLevel
    : unsigned int
{
    Debug,
    Info,
    Warning,
    Error,
    _Count
};

struct LogEntry
{
    std::chrono::system_clock::time_point timestamp;
    LogLevel level;
    std::string message;
};

using LogLevelMask = std::uint32_t;

constexpr LogLevelMask logLevelMask(LogLevel level)
{
    return 1u << static_cast<std::underlying_type_t<LogLevel>>(level);
}

struct LogEntryReference
{
    LogLevel level;
    size_t index;
};

class LogStore;

struct LogRefreshCallback
{
    virtual ~LogRefreshCallback() = default;

    virtual void before_append(size_t row) = 0;
    virtual void after_append() = 0;
};

class LogStoreFilter
{
    friend class LogStore;

private:
    const LogStore& store_;
    LogLevelMask mask_;
    std::vector<LogEntryReference> chronology_;
    size_t store_version_ = 0;
    size_t end_index_ = 0;

public:
    LogStoreFilter(const LogStore& store, LogLevelMask mask);

    LogLevelMask mask() const;

    size_t size() const;
    const LogEntry& operator[](size_t index) const;

    bool invalidated() const;
    void refresh(LogRefreshCallback* callback = nullptr);

private:
    void append_missing(LogRefreshCallback* callback = nullptr);
    void assert_valid() const;
};

class LogStore
{
    friend class LogStoreFilter;

private:
    std::array<std::vector<LogEntry>, static_cast<size_t>(LogLevel::_Count)> entry_lists_;
    std::vector<LogEntryReference> chronology_;
    size_t version_ = 0;

public:
    void add_entry(LogEntry entry);
    void clear();

    std::unique_ptr<LogStoreFilter> filter(LogLevelMask mask = ~0) const;
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

    LogLine debug();
    LogLine info();
    LogLine warning();
    LogLine error();

private:
    void submit(LogLevel level, std::string message);
    std::unique_lock<std::mutex> acquire_lock() const;
};

} // crucible::core

namespace crucible
{

extern core::Logger LOG;

} // crucible
