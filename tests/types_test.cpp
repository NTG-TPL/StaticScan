#include "stdx/types.hpp"

#include <cstddef>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

namespace {

using stdx::details::fixed_string;
using stdx::details::parse_error;
using stdx::details::scan_result;
using namespace std::string_view_literals;

constexpr fixed_string literal = "abc";
static_assert(std::is_same_v<decltype(literal), const fixed_string<4>>, "fixed_string: N deduced from literal");
static_assert(literal.view() == "abc"sv && literal.data[3] == '\0', "fixed_string: copies the array of the same size");

constexpr fixed_string<8> padded{"abc"};
static_assert(padded.view() == "abc\0\0\0\0"sv && padded.data[7] == '\0', "fixed_string: copies a smaller array");
static_assert(!std::is_constructible_v<fixed_string<2>, const char (&)[4]>,
              "fixed_string: rejects an array that does not fit");

constexpr char sentence[] = "hello world";
static_assert(fixed_string<6>{sentence + 6, sentence + 11}.view() == "world"sv, "fixed_string: copies a pointer range");
static_assert(fixed_string<1>{sentence, sentence}.view().empty(), "fixed_string: copies an empty pointer range");

template <fixed_string str>
constexpr std::size_t length_of = str.view().size();
static_assert(length_of<"four"> == 4, "fixed_string: usable as a template argument");

constexpr parse_error error{"Closing placeholder hasn't been found in appropriate place"};
static_assert(std::is_base_of_v<fixed_string<stdx::details::parse_error_capacity>, parse_error>,
              "parse_error: derives from fixed_string");
static_assert(error.message() == "Closing placeholder hasn't been found in appropriate place"sv,
              "parse_error: message() returns the text without trailing zeros");

static_assert(std::is_same_v<decltype(std::declval<const scan_result<int> &>().values()), const std::tuple<int> &>,
              "scan_result: values() of an lvalue returns a reference");
static_assert(std::is_same_v<decltype(std::declval<scan_result<int>>().values()), std::tuple<int>>,
              "scan_result: values() of an rvalue returns a value");
static_assert(std::is_same_v<scan_result<const int, volatile unsigned>::values_type, std::tuple<int, unsigned>>,
              "scan_result: drops cv-qualifiers");
static_assert(scan_result<int, std::string_view>{{1, "a"}}.values() == std::tuple{1, "a"sv},
              "scan_result: stores values in order");
static_assert(scan_result<int>{{1}} == scan_result<int>{{1}} && scan_result<int>{{1}} != scan_result<int>{{2}},
              "scan_result: compares values");

}  // namespace
