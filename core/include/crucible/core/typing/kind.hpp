#pragma once

#include <stdint.h>

namespace crucible::core::typing
{

enum class Kind
{
    None,

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

using UserInstance = uint32_t;

} // crucible::core::typing
