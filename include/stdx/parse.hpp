#pragma once

#include <charconv>
#include <cstddef>
#include <expected>
#include <string_view>
#include <system_error>
#include <type_traits>

#include "format_string.hpp"
#include "match.hpp"
#include "scannable.hpp"
#include "types.hpp"

namespace stdx::details {

template <std::size_t I, format_string fmt, fixed_string source>
consteval value_bounds get_current_source_for_parsing() {
    static_assert(I < fmt.number_placeholders, "Invalid placeholder index");
    return value_bounds_of<fmt, source>[I];
}

template <std::size_t I, format_string fmt>
consteval char get_specifier() {
    const auto [open, close] = fmt.placeholder_positions[I];
    return close - open == placeholder::specifier_length + 1 ? fmt.fmt.data[open + placeholder::specifier_length]
                                                             : specifier::none;
}

template <scannable_integer T>
consteval std::expected<T, std::errc> to_integer(std::string_view text) {
    T value{};
    const char *const last = text.data() + text.size();
    const auto [ptr, ec] = std::from_chars(text.data(), last, value);
    if (ec != std::errc{}) {
        return std::unexpected(ec);
    }
    if (ptr != last) {
        return std::unexpected(std::errc::invalid_argument);
    }
    return value;
}

template <scannable_integer T, fixed_string str>
    requires cv_unqualified<T>
consteval T parse_value() {
    constexpr auto result = to_integer<T>(str.view());
    static_assert(result || result.error() != std::errc::result_out_of_range,
                  "The value is out of range of the target type");
    static_assert(result || result.error() != std::errc::invalid_argument, "The value is not a valid integer");
    return result.value_or(T{});
}

template <scannable_string T, fixed_string str>
    requires cv_unqualified<T>
consteval T parse_value() {
    return str.view();
}

template <std::size_t I, format_string fmt, fixed_string source, scannable T>
consteval std::remove_cv_t<T> parse_input() {
    using value_type = std::remove_cv_t<T>;

    constexpr auto bounds = get_current_source_for_parsing<I, fmt, source>();
    constexpr fixed_string<bounds.end - bounds.start + 1> value{source.data + bounds.start, source.data + bounds.end};

    constexpr auto specifier_check = specifier::all::check<T>(get_specifier<I, fmt>());
    if constexpr (!specifier_check.accepted) {
        static_assert(false, specifier_check.error);
    } else {
        return parse_value<value_type, value>();
    }

    return value_type{};
}

}  // namespace stdx::details
