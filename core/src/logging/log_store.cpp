#include <crucible/core/logging/log_store.hpp>

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

bool LogStoreFilter::invalidated() const
{
    return store_version_ != store_.version_;
}

void LogStoreFilter::refresh(LogRefreshCallback* callback)
{
    if (invalidated())
    {
        chronology_.clear();
        end_index_ = 0;
        store_version_ = store_.version_;
    }

    append_missing(callback);
}

void LogStoreFilter::append_missing(LogRefreshCallback* callback)
{
    while (end_index_ < store_.chronology_.size())
    {
        LogEntryReference ref = store_.chronology_[end_index_++];
        if (mask_ & ref.level)
        {
            if (callback)
                callback->before_append(chronology_.size());

            chronology_.push_back(ref);

            if (callback)
                callback->after_append();
        }
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

} // crucible::core
