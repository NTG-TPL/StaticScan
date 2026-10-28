#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <string_view>

#include "format_string.hpp"
#include "types.hpp"

namespace stdx::details {

struct value_bounds {
    std::size_t start;
    std::size_t end;

    constexpr bool operator==(const value_bounds &) const = default;
};

template <format_string fmt, fixed_string source>
consteval auto all_value_bounds() {
    constexpr std::string_view format_text = fmt.fmt.view();
    constexpr std::string_view source_text = source.view();
    constexpr auto &positions = fmt.placeholder_positions;

    std::array<value_bounds, positions.size()> bounds{};
    std::size_t start = positions.empty() ? 0 : positions[0].open;
    for (std::size_t i = 0; i < positions.size(); ++i) {
        const auto [open, close] = positions[i];
        const std::size_t next = i + 1 < positions.size() ? positions[i + 1].open : format_text.size();
        const std::string_view sep = format_text.substr(close + 1, next - close - 1);

        const std::size_t end = close + 1 == format_text.size()
                                    ? source_text.size()
                                    : std::min(source_text.find(sep, start), source_text.size());
        bounds[i] = {.start = start, .end = end};
        start = std::min(end + sep.size(), source_text.size());
    }
    return bounds;
}

template <format_string fmt, fixed_string source>
inline constexpr auto value_bounds_of = all_value_bounds<fmt, source>();

template <format_string fmt>
consteval bool placeholders_are_separated() {
    constexpr auto &positions = fmt.placeholder_positions;
    return std::ranges::adjacent_find(positions,
                                      [](const placeholder_position &prev, const placeholder_position &next) {
                                          return next.open == prev.close + 1;
                                      }) == positions.end();
}

template <format_string fmt, fixed_string source>
consteval bool source_matches_format() {
    constexpr std::string_view format_text = fmt.fmt.view();
    constexpr std::string_view source_text = source.view();
    constexpr auto &positions = fmt.placeholder_positions;
    constexpr auto &bounds = value_bounds_of<fmt, source>;

    const auto same_text = [&](std::size_t fmt_from, std::size_t fmt_to, std::size_t src_from, std::size_t src_to) {
        return src_from <= src_to && src_to <= source_text.size() &&
               format_text.substr(fmt_from, fmt_to - fmt_from) == source_text.substr(src_from, src_to - src_from);
    };

    std::size_t fmt_from = 0;
    std::size_t src_from = 0;
    for (std::size_t i = 0; i < positions.size(); ++i) {
        const auto [open, close] = positions[i];
        const auto [start, end] = bounds[i];
        if (!same_text(fmt_from, open, src_from, start)) {
            return false;
        }
        fmt_from = close + 1;
        src_from = end;
    }
    return same_text(fmt_from, format_text.size(), src_from, source_text.size());
}

}  // namespace stdx::details
