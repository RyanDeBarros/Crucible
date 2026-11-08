#pragma once

class QObject;

namespace crucible::gui
{

class BlockSignals
{
    QObject& object_;
    bool was_blocked_;

public:
    BlockSignals(QObject& object);
    BlockSignals(const BlockSignals&) = delete;
    BlockSignals(BlockSignals&&) = delete;
    ~BlockSignals();

    BlockSignals& operator=(const BlockSignals&) = delete;
    BlockSignals& operator=(BlockSignals&&) = delete;

    operator bool() const;
};

} // crucible::gui
