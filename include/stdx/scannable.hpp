#pragma once

#include <algorithm>
#include <array>
#include <concepts>
#include <cstdint>
#include <iterator>
#include <string_view>
#include <type_traits>

#include "types.hpp"

namespace stdx::details {

template <typename T, typename... Us>
concept same_as_any = (std::same_as<T, Us> || ...);

template <typename T>
concept scannable_signed = same_as_any<std::remove_cv_t<T>, std::int8_t, std::int16_t, std::int32_t, std::int64_t>;

template <typename T>
concept scannable_unsigned =
    same_as_any<std::remove_cv_t<T>, std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>;

template <typename T>
concept scannable_integer = scannable_signed<T> || scannable_unsigned<T>;

template <typename T>
concept scannable_string = std::same_as<std::remove_cv_t<T>, std::string_view>;

template <typename T>
concept cv_unqualified = std::same_as<T, std::remove_cv_t<T>>;

namespace specifier {

// TODO :: %c %i %e %f %g %o %x %p %n %u %[] %%

inline constexpr char none = '\0';

struct check_result {
    bool accepted;
    std::string_view error;
};

template <typename... Specifiers>
struct list {
    static constexpr std::array letters{Specifiers::letter...};

    template <typename T>
    static constexpr bool accepts_any = (Specifiers::template accepts<T> || ...);

    template <typename T>
    static consteval check_result check(char spec) {
        if (spec == none) {
            return {.accepted = true, .error = {}};
        }

        struct row {
            char letter;
            bool accepted;
            std::string_view error;
        };
        constexpr row rows[]{{Specifiers::letter, Specifiers::template accepts<T>, Specifiers::mismatch_error}...};
        const auto found = std::ranges::find(rows, spec, &row::letter);
        return found != std::end(rows) ? check_result{.accepted = found->accepted, .error = found->error}
                                       : check_result{.accepted = false, .error = "Unknown conversion specifier"};
    }
};

template <char letter_value, fixed_string error>
struct descriptor {
    static constexpr char letter = letter_value;
    static constexpr std::string_view mismatch_error = error.view();
};

struct signed_integer : descriptor<'d', "The %d specifier requires a signed integer type"> {
    template <typename T>
    static constexpr bool accepts = scannable_signed<T>;
};

struct unsigned_integer : descriptor<'u', "The %u specifier requires an unsigned integer type"> {
    template <typename T>
    static constexpr bool accepts = scannable_unsigned<T>;
};

struct string : descriptor<'s', "The %s specifier requires std::string_view"> {
    template <typename T>
    static constexpr bool accepts = scannable_string<T>;
};

using all = list<signed_integer, unsigned_integer, string>;

}  // namespace specifier

template <typename T>
concept scannable = specifier::all::accepts_any<T>;

}  // namespace stdx::details
