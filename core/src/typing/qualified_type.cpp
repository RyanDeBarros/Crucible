#include <crucible/core/typing/qualified_type.hpp>

#include <crucible/core/typing/nominal_registries.hpp>

size_t std::hash<crucible::core::typing::Simplex>::operator()(
    const crucible::core::typing::Simplex& s) const
{
    return std::hash<crucible::core::typing::Kind>{}(s.kind)
        ^ std::hash<crucible::core::typing::UserInstance>{}(s.user);
}

namespace crucible::core::typing
{

QualifiedType::QualifiedType(Simplex simplex)
    : simplex_(simplex)
{
}

QualifiedType::QualifiedType(const QualifiedType& o)
    : simplex_(o.simplex_)
{
    for (size_t i = 0; i < kTemplateCap; ++i)
        template_[i] = o.template_[i] ? o.template_[i]->clone() : nullptr;
}

QualifiedType::QualifiedType(QualifiedType&& o) noexcept
    : simplex_(std::move(o.simplex_))
{
    for (size_t i = 0; i < kTemplateCap; ++i)
        template_[i] = std::move(o.template_[i]);
}

QualifiedType& QualifiedType::operator=(const QualifiedType& o)
{
    if (this != &o)
    {
        simplex_ = o.simplex_;

        for (size_t i = 0; i < kTemplateCap; ++i)
            template_[i] = o.template_[i] ? o.template_[i]->clone() : nullptr;
    }

    return *this;
}

QualifiedType& QualifiedType::operator=(QualifiedType&& o) noexcept
{
    if (this != &o)
    {
        simplex_ = o.simplex_;

        for (size_t i = 0; i < kTemplateCap; ++i)
            template_[i] = std::move(o.template_[i]);
    }

    return *this;
}

bool QualifiedType::operator==(const QualifiedType& o) const
{
    if (simplex_ != o.simplex_)
        return false;

    for (size_t i = 0; i < kTemplateCap; ++i)
    {
        if (template_[i] && o.template_[i])
        {
            if (!(*template_[i] == *o.template_[i]))
                return false;
        }
        else if (template_[i] || o.template_[i])
            return false;
    }

    return true;
}

std::unique_ptr<QualifiedType> QualifiedType::clone() const
{
    return std::unique_ptr<QualifiedType>(new QualifiedType(*this));
}

Simplex QualifiedType::simplex() const
{
    return simplex_;
}

const QualifiedType* QualifiedType::underlying(size_t idx) const
{
    return idx < kTemplateCap ? template_[idx].get() : nullptr;
}

size_t QualifiedType::hash() const
{
    return std::hash<Simplex>{}(simplex_);
}

QualifiedType QualifiedType::make_bool()
{
    return QualifiedType({ .kind = Kind::Bool });
}

QualifiedType QualifiedType::make_int()
{
    return QualifiedType({ .kind = Kind::Int });
}

QualifiedType QualifiedType::make_float()
{
    return QualifiedType({ .kind = Kind::Float });
}

QualifiedType QualifiedType::make_string()
{
    return QualifiedType({ .kind = Kind::String });
}

QualifiedType QualifiedType::make_pointer(QualifiedType underlying_type)
{
    QualifiedType type({ .kind = Kind::Pointer });
    type.template_[0] = std::unique_ptr<QualifiedType>(new QualifiedType(std::move(underlying_type)));
    return type;
}

QualifiedType QualifiedType::make_enum(UserInstance user)
{
    return QualifiedType({ .kind = Kind::Enum, .user = user });
}

QualifiedType QualifiedType::make_list(QualifiedType elem_type)
{
    QualifiedType type({ .kind = Kind::List });
    type.template_[0] = std::unique_ptr<QualifiedType>(new QualifiedType(std::move(elem_type)));
    return type;
}

QualifiedType QualifiedType::make_set(QualifiedType elem_type)
{
    QualifiedType type({ .kind = Kind::Set });
    type.template_[0] = std::unique_ptr<QualifiedType>(new QualifiedType(std::move(elem_type)));
    return type;
}

QualifiedType QualifiedType::make_map(QualifiedType key_type, QualifiedType value_type)
{
    QualifiedType type({ .kind = Kind::List });
    type.template_[0] = std::unique_ptr<QualifiedType>(new QualifiedType(std::move(key_type)));
    type.template_[1] = std::unique_ptr<QualifiedType>(new QualifiedType(std::move(value_type)));
    return type;
}

QualifiedType QualifiedType::make_struct(UserInstance user)
{
    return QualifiedType({ .kind = Kind::Struct, .user = user });
}

QualifiedType QualifiedType::make_joker()
{
    return QualifiedType({ .kind = Kind::Joker });
}

} // crucible::core::typing
