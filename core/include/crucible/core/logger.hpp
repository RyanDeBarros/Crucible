#pragma once

#include <array>
#include <chrono>
#include <mutex>
#include <sstream>
#include <vector>

namespace crucible::core
{

enum class LogLevel : unsigned int
{
    Debug,
    Info,
    Warning,
    Error,
    _Count
};

extern std::ostream& operator<<(std::ostream& os, LogLevel level);

struct LogCategory
{
    std::string text;

    bool operator==(const LogCategory&) const = default;
};

struct LogEntry
{
    std::chrono::system_clock::time_point timestamp;
    LogLevel level;
    LogCategory category;
    std::string message;
};

struct LogLevelMask
{
    uint32_t bits = 0;

    constexpr explicit LogLevelMask() = default;

    constexpr LogLevelMask(LogLevel level)
        : bits(1u << static_cast<std::underlying_type_t<LogLevel>>(level))
    {
    }

    constexpr bool operator==(const LogLevelMask& o) const
    {
        return bits == o.bits;
    }

    constexpr operator bool() const
    {
        return bits != 0;
    }

    constexpr LogLevelMask operator~() const
    {
        LogLevelMask mask{};
        mask.bits = ~bits;
        return mask;
    }

    constexpr LogLevelMask operator&(LogLevelMask o) const
    {
        LogLevelMask mask{};
        mask.bits = bits & o.bits;
        return mask;
    }

    constexpr LogLevelMask operator&(LogLevel level) const
    {
        return *this & LogLevelMask(level);
    }

    constexpr LogLevelMask& operator&=(LogLevelMask o)
    {
        bits &= o.bits;
        return *this;
    }

    constexpr LogLevelMask& operator&=(LogLevel level)
    {
        return *this &= LogLevelMask(level);
    }

    constexpr LogLevelMask operator|(LogLevelMask o) const
    {
        LogLevelMask mask{};
        mask.bits = bits | o.bits;
        return mask;
    }

    constexpr LogLevelMask operator|(LogLevel level) const
    {
        return *this | LogLevelMask(level);
    }

    constexpr LogLevelMask& operator|=(LogLevelMask o)
    {
        bits |= o.bits;
        return *this;
    }

    constexpr LogLevelMask& operator|=(LogLevel level)
    {
        return *this |= LogLevelMask(level);
    }

    constexpr static LogLevelMask full()
    {
        LogLevelMask mask{};
        for (size_t i = 0; i < static_cast<size_t>(LogLevel::_Count); ++i)
            mask.bits |= 1 << i;
        return mask;
    }
};

extern std::ostream& operator<<(std::ostream& os, LogLevelMask mask);

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

    std::unique_ptr<LogStoreFilter> filter(LogLevelMask mask = LogLevelMask::full()) const;
};

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
