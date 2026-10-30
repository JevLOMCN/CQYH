#include "number_parse.h"

#include <array>
#include <cstdio>
#include <string_view>
#include <utility>

int main()
{
    struct ValueCase { std::string_view input; std::int32_t expected; };
    constexpr std::array values{
        ValueCase{"0", 0}, ValueCase{"42", 42}, ValueCase{"-42", -42},
        ValueCase{"  +42tail", 42}, ValueCase{"\t\r\n\f\v-45 ", -45},
        ValueCase{"2147483647", 2147483647}, ValueCase{"-2147483648", (-2147483647 - 1)},
        ValueCase{"+2147483647", 2147483647}, ValueCase{"-00012abc", -12},
        ValueCase{"-0", 0}, ValueCase{"01", 1}, ValueCase{"0x10", 0},
        ValueCase{"3.25", 3}, ValueCase{"1e3", 1}, ValueCase{"0 1", 0}, ValueCase{"42suffix", 42}};
    struct ErrorCase { std::string_view input; std::errc expected; };
    constexpr std::array errors{
        ErrorCase{{}, std::errc::invalid_argument}, ErrorCase{" \t", std::errc::invalid_argument},
        ErrorCase{"word", std::errc::invalid_argument}, ErrorCase{"+", std::errc::invalid_argument},
        ErrorCase{"-", std::errc::invalid_argument}, ErrorCase{"+-1", std::errc::invalid_argument},
        ErrorCase{"-+1", std::errc::invalid_argument}, ErrorCase{"--1", std::errc::invalid_argument},
        ErrorCase{"+ 1", std::errc::invalid_argument},
        ErrorCase{"2147483648", std::errc::result_out_of_range},
        ErrorCase{"-2147483649", std::errc::result_out_of_range},
        ErrorCase{"+99999999999999999", std::errc::result_out_of_range},
        ErrorCase{"-99999999999999999tail", std::errc::result_out_of_range}};
    int checked = 0;
    int failures = 0;
    for (const auto& test : values)
    {
        const auto result = cqyh::ParseIntegerPrefix(test.input);
        ++checked;
        if (!result || *result != test.expected)
        {
            std::fprintf(stderr, "Incorrect numeric prefix: %.*s\n", static_cast<int>(test.input.size()), test.input.data());
            ++failures;
        }
    }
    for (const auto& test : errors)
    {
        const auto result = cqyh::ParseIntegerPrefix(test.input);
        ++checked;
        if (result || result.error() != test.expected)
        {
            std::fprintf(stderr, "Incorrect numeric error category\n");
            ++failures;
        }
    }
    constexpr std::array digits{'4', '2', '9', '9'};
    for (const auto [size, expected] : {std::pair{1U, 4}, std::pair{2U, 42}, std::pair{4U, 4299}})
    {
        const auto result = cqyh::ParseIntegerPrefix(std::string_view{digits.data(), size});
        ++checked;
        if (!result || *result != expected)
            ++failures;
    }
    std::printf("%d numeric checks, %d failures\n", checked, failures);
    return failures == 0 ? 0 : 1;
}
