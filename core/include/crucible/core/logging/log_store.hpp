#pragma once

#include <crucible/core/logging/log_category.hpp>
#include <crucible/core/logging/log_level.hpp>

#include <array>
#include <chrono>
#include <vector>

namespace crucible::core
{

struct LogEntry
{
    std::chrono::system_clock::time_point timestamp;
    LogLevel level;
    LogCategory category;
    std::string message;
};

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

} // crucible::core
