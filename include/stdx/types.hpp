#pragma once

#include <algorithm>
#include <cstddef>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

namespace stdx::details {

template <std::size_t N>
struct fixed_string {
    char data[N]{};

    constexpr fixed_string(const char (&str)[N]) { std::copy_n(str, N, data); }

    template <std::size_t M>
        requires(M < N)
    constexpr fixed_string(const char (&str)[M]) {
        std::copy_n(str, M, data);
    }

    constexpr fixed_string(const char *first, const char *last) { std::copy(first, last, data); }

    constexpr std::string_view view() const noexcept { return {data, N - 1}; }
};

inline constexpr std::size_t parse_error_capacity = 128;

struct parse_error : fixed_string<parse_error_capacity> {
    using fixed_string::fixed_string;

    constexpr std::string_view message() const noexcept { return data; }
};

template <typename... Ts>
class scan_result {
public:
    using values_type = std::tuple<std::remove_cvref_t<Ts>...>;

    constexpr explicit scan_result(values_type values) noexcept(nothrow_movable) : values_{std::move(values)} {}

    constexpr const values_type &values() const & noexcept { return values_; }

    constexpr values_type values() && noexcept(nothrow_movable) { return std::move(values_); }

    constexpr bool operator==(const scan_result &) const = default;

private:
    static constexpr bool nothrow_movable = std::is_nothrow_move_constructible_v<values_type>;

    values_type values_;
};

}  // namespace stdx::details
