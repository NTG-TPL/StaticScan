#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <optional>
#include <string_view>
#include <utility>

#include "format_string.hpp"
#include "match.hpp"
#include "parse.hpp"
#include "scannable.hpp"
#include "types.hpp"

namespace stdx::details {

struct requirement {
    bool (*is_met)();
    std::string_view error;
};

template <format_string fmt, fixed_string source, typename... Ts>
consteval auto scan_requirements() {
    return std::array{
        requirement{[] { return (scannable<Ts> && ...); },
                    "Supported types are int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t, "
                    "std::string_view and their cv-qualified versions"},
        requirement{[] { return fmt.is_valid; }, {} /* format_string has already reported the error */},
        requirement{[] { return fmt.number_placeholders == sizeof...(Ts); },
                    "The number of placeholders in the format string differs from the number of types"},
        requirement{&placeholders_are_separated<fmt>, "Placeholders in the format string must be separated by text"},
        requirement{&source_matches_format<fmt, source>,
                    "The text outside the placeholders differs in the format and source strings"},
    };
}

template <std::size_t N>
consteval std::optional<std::string_view> first_unmet(const std::array<requirement, N> &requirements) {
    const auto unmet = std::ranges::find_if_not(requirements, [](const requirement &r) { return r.is_met(); });
    return unmet != requirements.end() ? std::optional{unmet->error} : std::nullopt;
}

}  // namespace stdx::details

namespace stdx {

template <details::format_string fmt, details::fixed_string source, typename... Ts>
consteval details::scan_result<Ts...> scan() {
    using result_type = details::scan_result<Ts...>;
    using result_value_type = result_type::values_type;

    constexpr auto failure = details::first_unmet(details::scan_requirements<fmt, source, Ts...>());

    if constexpr (failure) {
        static_assert(failure->empty(), *failure);
    } else {
        return []<std::size_t... I>(std::index_sequence<I...>) {
            return result_type{result_value_type{details::parse_input<I, fmt, source, Ts>()...}};
        }(std::index_sequence_for<Ts...>{});
    }
    return result_type{result_value_type{}};
}

}  // namespace stdx
