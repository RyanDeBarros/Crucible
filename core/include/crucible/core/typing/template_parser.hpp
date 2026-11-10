#pragma once

#include <crucible/core/typing/compiler_type_name.hpp>

#include <vector>

namespace crucible::core::typing
{

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

constexpr size_t find_matching_template_end(std::string_view input, size_t opening)
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

constexpr TemplateNode parse_template_tree(std::string_view input)
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

} // crucible::core::typing::internal

template<typename T>
const TemplateNode& compiler_type_tree()
{
    return internal::compiler_type_tree_storage<T>;
}

} // crucible::core::typing
