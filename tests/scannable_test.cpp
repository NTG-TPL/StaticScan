#include "stdx/scannable.hpp"

#include <algorithm>
#include <cstdint>
#include <string>
#include <string_view>

namespace {

using stdx::details::cv_unqualified;
using stdx::details::scannable;
using stdx::details::scannable_signed;
using stdx::details::scannable_string;
using stdx::details::scannable_unsigned;
namespace specifier = stdx::details::specifier;

static_assert(scannable<std::int8_t> && scannable<std::int16_t> && scannable<std::int32_t> && scannable<std::int64_t>,
              "scannable: signed integers");
static_assert(scannable<std::uint8_t> && scannable<std::uint16_t> && scannable<std::uint32_t> &&
                  scannable<std::uint64_t>,
              "scannable: unsigned integers");
static_assert(scannable<std::string_view> && scannable<const std::int32_t> && scannable<volatile std::uint8_t> &&
                  scannable<const volatile std::string_view>,
              "scannable: std::string_view and cv-qualified types");
static_assert(!scannable<bool> && !scannable<char> && !scannable<float> && !scannable<double> &&
                  !scannable<std::string> && !scannable<const char *>,
              "scannable: other types are rejected");
static_assert(!scannable<int &> && !scannable<const std::string_view &> && !scannable<std::uint8_t &&>,
              "scannable: references are rejected");

static_assert(scannable_signed<std::int64_t> && !scannable_unsigned<std::int64_t> && !scannable_string<std::int64_t>,
              "scannable: signed integers are only signed");
static_assert(scannable_unsigned<std::uint8_t> && !scannable_signed<std::uint8_t> && !scannable_string<std::uint8_t>,
              "scannable: unsigned integers are only unsigned");

static_assert(cv_unqualified<int> && !cv_unqualified<const int> && !cv_unqualified<volatile int>, "cv_unqualified");

constexpr auto has_letter = [](char letter) { return std::ranges::contains(specifier::all::letters, letter); };
static_assert(has_letter('d') && has_letter('u') && has_letter('s'), "specifiers: letters of the supported specifiers");

static_assert(specifier::all::check<const std::int8_t>('d').accepted &&
                  specifier::all::check<std::uint64_t>('u').accepted &&
                  specifier::all::check<std::string_view>('s').accepted,
              "specifier::all::check: matching types");
static_assert(specifier::all::check<std::int32_t>(specifier::none).accepted &&
                  specifier::all::check<std::string_view>(specifier::none).accepted,
              "specifier::all::check: a placeholder without a specifier accepts any type");
static_assert(!specifier::all::check<std::int32_t>('u').accepted &&
                  specifier::all::check<std::int32_t>('u').error == specifier::unsigned_integer::mismatch_error,
              "specifier::all::check: mismatch gives the descriptor's error");
static_assert(!specifier::all::check<std::uint32_t>('d').accepted &&
                  !specifier::all::check<std::string_view>('d').accepted,
              "specifier::all::check: other mismatches");
static_assert(!specifier::all::check<std::int32_t>('x').accepted &&
                  specifier::all::check<std::int32_t>('x').error == "Unknown conversion specifier",
              "specifier::all::check: unknown letter is rejected");

}  // namespace
