#include <crucible/core/typing/qualified_type.hpp>

namespace crucible::core::typing
{

QualifiedType::QualifiedType(
    Kind base_type,
    std::unique_ptr<QualifiedType> key_type,
    std::unique_ptr<QualifiedType> value_type,
    KindInstance instance)
    : base_type_(base_type)
    , key_type_(std::move(key_type))
    , value_type_(std::move(value_type))
    , instance_(instance)
{
}

QualifiedType::QualifiedType(const QualifiedType& o)
    : base_type_(o.base_type_)
    , key_type_(o.key_type_ ? o.key_type_->clone() : nullptr)
    , value_type_(o.value_type_ ? o.value_type_->clone() : nullptr)
    , instance_(o.instance_)
{
}

QualifiedType::QualifiedType(QualifiedType&& o) noexcept
    : base_type_(o.base_type_)
    , key_type_(std::move(o.key_type_))
    , value_type_(std::move(o.value_type_))
    , instance_(o.instance_)
{
}

QualifiedType& QualifiedType::operator=(const QualifiedType& o)
{
    if (this != &o)
    {
        base_type_ = o.base_type_;
        key_type_ = o.key_type_ ? o.key_type_->clone() : nullptr;
        value_type_ = o.value_type_ ? o.value_type_->clone() : nullptr;
        instance_ = o.instance_;
    }

    return *this;
}

QualifiedType& QualifiedType::operator=(QualifiedType&& o) noexcept
{
    if (this != &o)
    {
        base_type_ = o.base_type_;
        key_type_ = std::move(o.key_type_);
        value_type_ = std::move(o.value_type_);
        instance_ = o.instance_;
    }

    return *this;
}

template<typename T>
static bool equal_ptrs(const std::unique_ptr<T>& a, const std::unique_ptr<T>& b)
{
    return (a && b) ? (*a == *b) : (!a && !b);
}

template<typename T>
static bool not_equal_ptrs(const std::unique_ptr<T>& a, const std::unique_ptr<T>& b)
{
    return (a && b) ? (*a != *b) : (a || b);
}

bool QualifiedType::operator==(const QualifiedType& o) const
{
    return base_type_ == o.base_type_
        && instance_ == o.instance_
        && equal_ptrs(key_type_, o.key_type_)
        && equal_ptrs(value_type_, o.value_type_);
}

bool QualifiedType::operator!=(const QualifiedType& o) const
{
    return base_type_ != o.base_type_
        || instance_ != o.instance_
        || not_equal_ptrs(key_type_, o.key_type_)
        || not_equal_ptrs(value_type_, o.value_type_);
}

Kind QualifiedType::base_type() const
{
    return base_type_;
}

const QualifiedType* QualifiedType::key_type() const
{
    return key_type_ ? key_type_.get() : nullptr;
}

const QualifiedType* QualifiedType::value_type() const
{
    return value_type_ ? value_type_.get() : nullptr;
}

KindInstance QualifiedType::instance() const
{
    return instance_;
}

std::unique_ptr<QualifiedType> QualifiedType::clone() const
{
    return std::unique_ptr<QualifiedType>(new QualifiedType(
        base_type_,
        key_type_ ? key_type_->clone() : nullptr,
        value_type_ ? value_type_->clone() : nullptr,
        instance_));
}

std::unique_ptr<QualifiedType> QualifiedType::to_ptr() &&
{
    return std::unique_ptr<QualifiedType>(new QualifiedType(
        base_type_,
        std::move(key_type_),
        std::move(value_type_),
        instance_));
}

QualifiedType QualifiedType::make_bool()
{
    return QualifiedType(Kind::Bool, nullptr, nullptr, 0);
}

QualifiedType QualifiedType::make_int()
{
    return QualifiedType(Kind::Int, nullptr, nullptr, 0);
}

QualifiedType QualifiedType::make_float()
{
    return QualifiedType(Kind::Float, nullptr, nullptr, 0);
}

QualifiedType QualifiedType::make_string()
{
    return QualifiedType(Kind::String, nullptr, nullptr, 0);
}

QualifiedType QualifiedType::make_pointer(QualifiedType pointer_type)
{
    return QualifiedType(Kind::Pointer, std::move(pointer_type).to_ptr(), nullptr, 0);
}

QualifiedType QualifiedType::make_enum(KindInstance instance)
{
    return QualifiedType(Kind::Enum, nullptr, nullptr, instance);
}

QualifiedType QualifiedType::make_list(QualifiedType elem_type)
{
    return QualifiedType(Kind::List, std::move(elem_type).to_ptr(), nullptr, 0);
}

QualifiedType QualifiedType::make_set(QualifiedType elem_type)
{
    return QualifiedType(Kind::Set, std::move(elem_type).to_ptr(), nullptr, 0);
}

QualifiedType QualifiedType::make_map(QualifiedType key_type, QualifiedType map_type)
{
    return QualifiedType(Kind::Map, std::move(key_type).to_ptr(), std::move(map_type).to_ptr(), 0);
}

QualifiedType QualifiedType::make_struct(KindInstance instance)
{
    return QualifiedType(Kind::Struct, nullptr, nullptr, instance);
}

QualifiedType QualifiedType::make_joker()
{
    return QualifiedType(Kind::Joker, nullptr, nullptr, 0);
}

} // crucible::core::typing
