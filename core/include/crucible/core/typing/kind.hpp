#pragma once

namespace crucible::core::typing
{

enum class Kind
{
    Bool,
    Int,
    Float,
    String,
    Pointer,
    Enum,

    List,
    Set,
    Map,
    Struct,

    Joker
};

using Specialization = size_t;

} // crucible::core::typing
