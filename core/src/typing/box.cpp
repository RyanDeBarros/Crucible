#include <crucible/core/typing/box.hpp>

#include <stdexcept>

namespace crucible::core::typing
{

Box::Box(const Box& o)
    : raw_(o.try_copy(o.raw_))
    , type_(o.type_)
{
    if (o.raw_ && !raw_)
        throw std::logic_error("Attempted to copy a non-copyable type");
}

Box::Box(Box&& o) noexcept
    : raw_(o.raw_)
    , type_(o.type_)
{
    o.raw_ = nullptr;
    o.type_ = {};
}

Box::~Box()
{
    try_dtor(raw_);
}

Box& Box::operator=(const Box& o)
{
    if (this != &o)
    {
        if (type_ != o.type_ || !try_copy_assign(raw_, o.raw_))
        {
            try_dtor(raw_);
            raw_ = o.try_copy(o.raw_);
        }

        type_ = o.type_;

        if (o.raw_ && !raw_)
            throw std::logic_error("Attempted to copy a non-copyable type");
    }

    return *this;
}

Box& Box::operator=(Box&& o) noexcept
{
    if (this != &o)
    {
        if (type_ != o.type_ || !try_move_assign(raw_, o.raw_))
        {
            try_dtor(raw_);
            raw_ = o.raw_;
        }

        type_ = o.type_;

        o.raw_ = nullptr;
        o.type_ = {};
    }

    return *this;
}

bool Box::operator==(const Box& o) const
{
    if (raw_ == nullptr || o.raw_ == nullptr)
        return raw_ == o.raw_;

    if (type_ != o.type_)
        return false;

    if (auto* equal_op = type_.desc().equal_op_)
        return (*equal_op)(raw_, o.raw_);
    else
        throw std::runtime_error("Equality not defined for this type");
}

bool Box::operator!=(const Box& o) const
{
    if (raw_ == nullptr || o.raw_ == nullptr)
        return raw_ != o.raw_;

    if (type_ != o.type_)
        return true;

    if (auto* not_equal_op = type_.desc().not_equal_op_)
        return (*not_equal_op)(raw_, o.raw_);
    else
        throw std::runtime_error("Inequality not defined for this type");
}

bool Box::holds(TypeErasure type) const
{
    return type_ == type;
}

TypeErasure Box::type() const
{
    return type_;
}

const void* Box::unsafe_raw() const
{
    return raw_;
}

void* Box::unsafe_raw()
{
    return raw_;
}

size_t Box::hash() const
{
    if (auto* hasher = type_.desc().hasher_op_)
        return (*hasher)(raw_);
    else
        throw std::runtime_error("Hashing not defined for this type");
}

void Box::try_dtor(void* ptr) const
{
    if (auto* dtor = type_.desc().dtor_op_)
        (*dtor)(ptr);
}

void* Box::try_copy(const void* ptr) const
{
    if (auto* copy = type_.desc().copy_op_)
        return (*copy)(ptr);
    else
        return nullptr;
}

bool Box::try_copy_assign(void* to, const void* from)
{
    if (to && from)
    {
        if (auto* copy_assign = type_.desc().copy_assign_op_)
        {
            (*copy_assign)(to, from);
            return true;
        }
    }

    return false;
}

bool Box::try_move_assign(void* to, void* from)
{
    if (to && from)
    {
        if (auto* move_assign = type_.desc().move_assign_op_)
        {
            (*move_assign)(to, from);
            return true;
        }
    }

    return false;
}

} // crucible::core::typing

size_t std::hash<crucible::core::typing::Box>::operator()(
    const crucible::core::typing::Box& box) const
{
    return box.hash();
}
