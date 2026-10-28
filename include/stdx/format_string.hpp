#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <expected>
#include <string_view>

#include "scannable.hpp"
#include "types.hpp"

namespace stdx::details {

namespace placeholder {
inline constexpr char open = '{';
inline constexpr char close = '}';
inline constexpr char marker = '%';
inline constexpr std::size_t specifier_length = 2;  // % + letter (for example %d, %u, %s, %f .. etc)
}  // namespace placeholder

struct placeholder_position {
    std::size_t open;
    std::size_t close;

    constexpr bool operator==(const placeholder_position &) const = default;
};

template <fixed_string str>
class format_string {

    static consteval std::expected<std::size_t, parse_error> get_number_placeholders();

    template <std::size_t count>
    static consteval std::array<placeholder_position, count> get_placeholder_positions();

    static constexpr auto parsed_placeholders = get_number_placeholders();

public:
    static constexpr auto fmt = str;

    static constexpr bool is_valid = parsed_placeholders.has_value();

    static constexpr std::size_t number_placeholders = [] {
        if constexpr (!is_valid) {
            static_assert(false, parsed_placeholders.error().message());
        }
        return parsed_placeholders.value_or(0);
    }();

    static constexpr auto placeholder_positions = get_placeholder_positions<number_placeholders>();
};

template <fixed_string str>
consteval std::expected<std::size_t, parse_error> format_string<str>::get_number_placeholders() {
    constexpr std::string_view text = str.view();
    std::size_t placeholder_count = 0;
    for (std::size_t open = text.find(placeholder::open); open != std::string_view::npos;
         open = text.find(placeholder::open, open)) {
        ++placeholder_count;
        std::string_view rest = text.substr(open + 1);
        const bool has_specifier = rest.starts_with(placeholder::marker);
        if (rest.empty() || (has_specifier && rest.size() < placeholder::specifier_length)) {
            return std::unexpected(parse_error{"Unclosed last placeholder"});
        }
        if (has_specifier) {
            if (!std::ranges::contains(specifier::all::letters, rest[placeholder::specifier_length - 1])) {
                return std::unexpected(parse_error{"Invalid specifier"});
            }
            rest.remove_prefix(placeholder::specifier_length);
        }
        if (!rest.starts_with(placeholder::close)) {
            return std::unexpected(parse_error{"Closing placeholder hasn't been found in appropriate place"});
        }
        const std::size_t close = text.size() - rest.size();
        open = close + 1;
    }
    return placeholder_count;
}

template <fixed_string str>
template <std::size_t count>
consteval std::array<placeholder_position, count> format_string<str>::get_placeholder_positions() {
    constexpr std::string_view text = str.view();
    std::array<placeholder_position, count> positions{};
    std::size_t open = text.find(placeholder::open);
    for (auto &position : positions) {
        const std::size_t close = text.find(placeholder::close, open);
        position = {.open = open, .close = close};
        open = text.find(placeholder::open, close);
    }
    return positions;
}

}  // namespace stdx::details

namespace stdx::literals {

template <details::fixed_string str>
consteval auto operator""_fs() {
    return details::format_string<str>{};
}

}  // namespace stdx::literals
