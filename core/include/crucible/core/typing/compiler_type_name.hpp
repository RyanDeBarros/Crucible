#pragma once

#include <string>
#include <string_view>

// TODO put compiler_type_name, template_parser, box, and type_erasure into a reflection/ folder. Eventually put this into a single reflection.hpp single-file repo so it can be dropped in imperious as well. No need for cmake - just drop the file in. Can use 'refl' namespace.

namespace crucible::core::typing
{

namespace internal
{

#if defined(_MSC_VER)
#define REFLECTION_FUNCTION_SIGNATURE __FUNCSIG__
#else
#define REFLECTION_FUNCTION_SIGNATURE __PRETTY_FUNCTION__
#endif

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
    return extract_type_name(REFLECTION_FUNCTION_SIGNATURE);
}

#undef REFLECTION_FUNCTION_SIGNATURE

constexpr bool is_identifier_char(char c) noexcept
{
    return (c >= 'a' && c <= 'z')
        || (c >= 'A' && c <= 'Z')
        || (c >= '0' && c <= '9')
        || c == '_';
}

constexpr bool is_elaborated_keyword(
    std::string_view token) noexcept
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
constexpr auto compiler_type_name_storage =
    normalize_type_name<raw_compiler_type_name<T>().size()>(raw_compiler_type_name<T>());

template<typename T>
constexpr std::string_view compiler_type_name_impl() noexcept
{
    return compiler_type_name_storage<T>.view();
}

} // crucible::core::typing::internal

template<typename T>
constexpr std::string_view compiler_type_name() noexcept
{
    return internal::compiler_type_name_impl<T>();
}

} // crucible::core::typing
