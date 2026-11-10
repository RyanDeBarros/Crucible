#pragma once

// TODO move to own repo so it can be dropped in imperious

#include <cstddef>
#include <cstdint>
#include <concepts>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace reflector
{

// ================================ PUBLIC API ================================

template<typename T>
constexpr std::string_view compiler_type_name() noexcept;

struct TemplateNode;

template<typename T>
const TemplateNode& compiler_type_tree() noexcept;

struct TypeErasure;

template<typename T>
constexpr TypeErasure erase_type() noexcept;

template<typename T, typename U>
constexpr bool erases_to() noexcept;

template<typename T>
constexpr const T* resolve_type(TypeErasure erasure, const void* ptr) noexcept;

template<typename T>
constexpr T* resolve_type(TypeErasure erasure, void* ptr) noexcept;

template<typename T>
constexpr const T* resolve_type(TypeErasure erasure, const T* ptr) noexcept;

template<typename T>
constexpr T* resolve_type(TypeErasure erasure, T* ptr) noexcept;

class Box;

template<typename T, typename... Args> requires (!std::is_void_v<T>)
Box make_box(Args&&... args);

template<typename T, typename... Args> requires (!std::is_void_v<T>)
void copy_box(Box& b, Args&&... args);

template<typename T>
Box forward_to_box(T&& obj);

template<typename T>
void forward_into_box(Box& b, T&& obj);

// ================================= INTERNAL =================================

namespace internal
{

constexpr std::string_view extract_type_name(std::string_view signature) noexcept
{
#if defined(_MSC_VER)

    constexpr std::string_view prefix = "raw_compiler_type_name<";

    const auto begin = signature.find(prefix);
    if (begin == std::string_view::npos)
        return {};

    const auto start = begin + prefix.size();
    const auto end = signature.find(">(", start);

    if (end == std::string_view::npos)
        return {};

    auto result = signature.substr(start, end - start);

#else

    constexpr std::string_view prefix = "T = ";

    const auto begin = signature.find(prefix);
    if (begin == std::string_view::npos)
        return {};

    const auto start = begin + prefix.size();

    auto end = signature.find(';', start);
    if (end == std::string_view::npos)
        end = signature.find(']', start);

    if (end == std::string_view::npos)
        return {};

    auto result = signature.substr(start, end - start);

#endif

    return result;
}

template<typename T>
constexpr std::string_view raw_compiler_type_name()
{
#if defined(_MSC_VER)
    return extract_type_name(__FUNCSIG__);
#else
    return extract_type_name(__PRETTY_FUNCTION__);
#endif
}

constexpr bool is_identifier_char(char c) noexcept
{
    return (c >= 'a' && c <= 'z')
        || (c >= 'A' && c <= 'Z')
        || (c >= '0' && c <= '9')
        || c == '_';
}

constexpr bool is_elaborated_keyword(std::string_view token) noexcept
{
    return token == "class"
        || token == "struct"
        || token == "enum"
        || token == "union";
}

template<size_t N>
struct static_string
{
    char data[N]{};
    size_t size = 0;

    constexpr std::string_view view() const noexcept
    {
        return { data, size };
    }
};

template<size_t N>
constexpr static_string<N> normalize_type_name(std::string_view input) noexcept
{
    static_string<N> result{};

    for (size_t i = 0; i < input.size();)
    {
        if (!is_identifier_char(input[i]))
        {
            result.data[result.size++] = input[i++];
            continue;
        }

        const auto start = i;

        while (i < input.size() && is_identifier_char(input[i]))
            ++i;

        const auto token = input.substr(start, i - start);
        if (is_elaborated_keyword(token))
            continue;

        for (const auto c : token)
            result.data[result.size++] = c;
    }

    return result;
}

template<typename T>
constexpr auto compiler_type_name_storage = normalize_type_name<raw_compiler_type_name<T>().size()>(
    raw_compiler_type_name<T>());

template<typename T>
constexpr std::string_view compiler_type_name_impl() noexcept
{
    return compiler_type_name_storage<T>.view();
}

} // internal

template<typename T>
constexpr std::string_view compiler_type_name() noexcept
{
    return internal::compiler_type_name_impl<T>();
}

struct TemplateNode
{
    std::string name;
    std::vector<TemplateNode> parameters;

    constexpr bool is_template() const noexcept
    {
        return !parameters.empty();
    }
};

namespace internal
{

constexpr bool is_whitespace(char c)
{
    return c == ' '
        || c == '\t'
        || c == '\n'
        || c == '\r';
}

constexpr std::string_view trim(std::string_view value) noexcept
{
    while (!value.empty() && is_whitespace(value.front()))
        value.remove_prefix(1);

    while (!value.empty() && is_whitespace(value.back()))
        value.remove_suffix(1);

    return value;
}

constexpr size_t find_matching_template_end(std::string_view input, size_t opening) noexcept
{
    size_t depth = 0;

    for (size_t i = opening; i < input.size(); ++i)
    {
        if (input[i] == '<')
            ++depth;
        else if (input[i] == '>')
        {
            if (--depth == 0)
                return i;
        }
    }

    return std::string_view::npos;
}

constexpr TemplateNode parse_template_tree(std::string_view input) noexcept
{
    input = trim(input);

    TemplateNode result;

    const auto opening = input.find('<');

    if (opening == std::string_view::npos)
    {
        result.name = input;
        return result;
    }

    const auto closing = find_matching_template_end(input, opening);

    if (closing == std::string_view::npos)
    {
        result.name = input;
        return result;
    }

    result.name = trim(input.substr(0, opening));

    const auto arguments = input.substr(opening + 1, closing - opening - 1);

    size_t start = 0;
    size_t depth = 0;

    for (size_t i = 0; i < arguments.size(); ++i)
    {
        if (arguments[i] == '<')
            ++depth;
        else if (arguments[i] == '>')
            --depth;
        else if (arguments[i] == ',' && depth == 0)
        {
            result.parameters.push_back(parse_template_tree(arguments.substr(start, i - start)));
            start = i + 1;
        }
    }

    if (start < arguments.size())
        result.parameters.push_back(parse_template_tree(arguments.substr(start)));

    return result;
}

template<typename T>
inline const auto compiler_type_tree_storage = parse_template_tree(compiler_type_name<T>());

} // internal

template<typename T>
const TemplateNode& compiler_type_tree() noexcept
{
    return internal::compiler_type_tree_storage<T>;
}

namespace internal
{

constexpr std::uint64_t fnv1a_hash(std::string_view str) noexcept
{
    std::uint64_t hash = 0xcbf29ce484222325ULL;
    for (char c : str)
    {
        hash ^= static_cast<std::uint64_t>(static_cast<unsigned char>(c));
        hash *= 0x00000100000001B3ULL;
    }
    return hash;
}

} // internal

struct TypeErasure
{
    template<typename T>
    friend constexpr TypeErasure erase_type() noexcept;

    struct Descriptor
    {
        friend class Box;
        friend class TypeErasure;

    public:
        std::string_view name;
        size_t type_hash;

    private:
        void (*dtor_op_)(void*);
        void* (*copy_op_)(const void*);
        void (*copy_assign_op_)(void*, const void*);
        void (*move_assign_op_)(void*, void*);
        size_t(*hasher_op_)(const void*);
        bool (*equal_op_)(const void*, const void*);
        bool (*not_equal_op_)(const void*, const void*);

    private:
        constexpr Descriptor(
            std::string_view name,
            size_t type_hash,
            void (*dtor_op)(void*),
            void* (*copy_op)(const void*),
            void (*copy_assign_op)(void*, const void*),
            void (*move_assign_op)(void*, void*),
            size_t(*hasher_op)(const void*),
            bool (*equal_op)(const void*, const void*),
            bool (*not_equal_op)(const void*, const void*)
        ) noexcept
            : name(name),
            type_hash(type_hash),
            dtor_op_(dtor_op),
            copy_op_(copy_op),
            copy_assign_op_(copy_assign_op),
            move_assign_op_(move_assign_op),
            hasher_op_(hasher_op),
            equal_op_(equal_op),
            not_equal_op_(not_equal_op)
        {
        }
    };

private:
    template<typename T>
    static constexpr Descriptor kDescriptor = Descriptor(
        compiler_type_name<T>(),
        internal::fnv1a_hash(compiler_type_name<T>()),
        []() {
        if constexpr (!std::is_void_v<T>)
        {
            return +[](void* ptr) {
                delete static_cast<T*>(ptr);
            };
        }
        else
            return nullptr;
    }(),
        []() {
        if constexpr (!std::is_void_v<T> && std::is_copy_constructible_v<T>)
        {
            return +[](const void* ptr) -> void* {
                return ptr ? new T(*static_cast<const T*>(ptr)) : nullptr;
            };
        }
        else
            return nullptr;
    }(),
        []() {
        if constexpr (!std::is_void_v<T> && std::is_copy_assignable_v<T>)
        {
            return +[](void* to, const void* from) {
                *static_cast<T*>(to) = *static_cast<const T*>(from);
            };
        }
        else
            return nullptr;
    }(),
        []() {
        if constexpr (!std::is_void_v<T> && std::is_move_assignable_v<T>)
        {
            return +[](void* to, void* from) {
                *static_cast<T*>(to) = std::move(*static_cast<T*>(from));
            };
        }
        else
            return nullptr;
    }(),
        []() {
        if constexpr (!std::is_void_v<T> && requires(const T & value)
        {
            {
                std::hash<T>{}(value)
            } -> std::convertible_to<size_t>;
        })
        {
            return +[](const void* ptr) {
                return std::hash<T>{}(*static_cast<const T*>(ptr));
            };
        }
        else
            return nullptr;
    }(),
        []() {
        if constexpr (!std::is_void_v<T> && requires(const T & a, const T & b)
        {
            {
                a == b
            } -> std::convertible_to<bool>;
        })
        {
            return +[](const void* lhs, const void* rhs) -> bool {
                return *static_cast<const T*>(lhs) == *static_cast<const T*>(rhs);
            };
        }
        else
            return nullptr;
    }(),
        []() {
        if constexpr (!std::is_void_v<T> && requires(const T & a, const T & b)
        {
            {
                a != b
            } -> std::convertible_to<bool>;
        })
        {
            return +[](const void* lhs, const void* rhs) -> bool {
                return *static_cast<const T*>(lhs) != *static_cast<const T*>(rhs);
            };
        }
        else
            return nullptr;
    }()
        );

    const Descriptor* desc_ = nullptr;

    constexpr explicit TypeErasure(const Descriptor* desc) noexcept
        : desc_(desc)
    {
    }

public:
    constexpr TypeErasure() noexcept
        : desc_(&kDescriptor<void>)
    {
    }

    constexpr bool operator==(const TypeErasure& o) const noexcept
    {
        return desc_ == o.desc_;
    }

    constexpr bool operator!=(const TypeErasure& o) const noexcept
    {
        return desc_ != o.desc_;
    }

    [[nodiscard]]
    constexpr size_t hash() const noexcept
    {
        return desc_->type_hash;
    }

    [[nodiscard]]
    constexpr std::string_view name() const noexcept
    {
        return desc_->name;
    }

    [[nodiscard]]
    constexpr const Descriptor& desc() const noexcept
    {
        return *desc_;
    }
};

template<typename T>
constexpr TypeErasure erase_type() noexcept
{
    return TypeErasure(&TypeErasure::kDescriptor<T>);
}

template<typename T, typename U>
constexpr bool erases_to() noexcept
{
    return erase_type<T>() == erase_type<U>();
}

template<typename T>
constexpr const T* resolve_type(TypeErasure erasure, const void* ptr) noexcept
{
    return erase_type<T>() == erasure ? static_cast<const T*>(ptr) : nullptr;
}

template<typename T>
constexpr T* resolve_type(TypeErasure erasure, void* ptr) noexcept
{
    return erase_type<T>() == erasure ? static_cast<T*>(ptr) : nullptr;
}

template<typename T>
constexpr const T* resolve_type(TypeErasure erasure, const T* ptr) noexcept
{
    return resolve_type<T>(erasure, static_cast<const void*>(ptr));
}

template<typename T>
constexpr T* resolve_type(TypeErasure erasure, T* ptr) noexcept
{
    return resolve_type<T>(erasure, static_cast<void*>(ptr));
}

class Box
{
    void* raw_ = nullptr;
    TypeErasure type_;

public:
    Box() = default;

    template<typename T>
    Box(T* raw)
        : raw_(raw)
        , type_(erase_type<T>())
    {
    }

    Box(const Box& o)
        : raw_(o.try_copy(o.raw_))
        , type_(o.type_)
    {
        if (o.raw_ && !raw_)
            throw std::logic_error("Attempted to copy a non-copyable type");
    }

    Box(Box&& o) noexcept
        : raw_(o.raw_)
        , type_(o.type_)
    {
        o.raw_ = nullptr;
        o.type_ = {};
    }

    ~Box()
    {
        try_dtor(raw_);
    }

    Box& operator=(const Box& o)
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

    Box& operator=(Box&& o) noexcept
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

    bool operator==(const Box& o) const
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

    bool operator!=(const Box& o) const
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

public:
    template<typename T>
    bool holds() const
    {
        return type_ == erase_type<T>();
    }

    bool holds(TypeErasure type) const
    {
        return type_ == type;
    }

    TypeErasure type() const
    {
        return type_;
    }

    const void* unsafe_raw() const
    {
        return raw_;
    }

    void* unsafe_raw()
    {
        return raw_;
    }

    template<typename T>
    const T* as() const
    {
        if (holds<T>())
            return static_cast<const T*>(raw_);
        else
            return nullptr;
    }

    template<typename T>
    T* as()
    {
        if (holds<T>())
            return static_cast<T*>(raw_);
        else
            return nullptr;
    }

    template<typename T>
    std::unique_ptr<T> consume()
    {
        if (holds<T>())
        {
            std::unique_ptr<T> ptr(static_cast<T*>(raw_));
            raw_ = nullptr;
            return ptr;
        }
        else
            return nullptr;
    }

    size_t hash() const
    {
        if (auto* hasher = type_.desc().hasher_op_)
            return (*hasher)(raw_);
        else
            throw std::runtime_error("Hashing not defined for this type");
    }

private:
    void try_dtor(void* ptr) const
    {
        if (auto* dtor = type_.desc().dtor_op_)
            (*dtor)(ptr);
    }

    void* try_copy(const void* ptr) const
    {
        if (auto* copy = type_.desc().copy_op_)
            return (*copy)(ptr);
        else
            return nullptr;
    }

    bool try_copy_assign(void* to, const void* from)
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

    bool try_move_assign(void* to, void* from)
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
};

template<typename T, typename... Args> requires (!std::is_void_v<T>)
Box make_box(Args&&... args)
{
    return Box(new T(std::forward<Args>(args)...));
}

template<typename T, typename... Args> requires (!std::is_void_v<T>)
void copy_box(Box& b, Args&&... args)
{
    if (T* obj = b.as<T>())
        *obj = T(std::forward<Args>(args)...);
    else
        b = make_box<T>(std::forward<Args>(args)...);
}

template<typename T>
Box forward_to_box(T&& obj)
{
    return make_box<std::remove_cvref_t<T>>(std::forward<T>(obj));
}

template<typename T>
void forward_into_box(Box& b, T&& obj)
{
    copy_box<std::remove_cvref_t<T>>(b, std::forward<T>(obj));
}

} // reflector

template<>
struct std::hash<reflector::TypeErasure>
{
    size_t operator()(reflector::TypeErasure te) const
    {
        return te.hash();
    }
};

template<>
struct std::hash<reflector::Box>
{
    size_t operator()(const reflector::Box& box) const
    {
        return box.hash();
    }
};
