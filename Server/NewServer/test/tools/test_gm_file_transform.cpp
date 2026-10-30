#include "base.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
std::size_t checks;
void check(bool value, const char* message)
{
    ++checks;
    if (!value)
        throw std::runtime_error(message);
}

// Independent bytewise oracle for the existing little-endian on-disk format.
std::vector<char> legacy_bytes(const std::vector<char>& input)
{
    std::vector<char> result((input.size() + 7) & ~std::size_t{3}, 0);
    for (unsigned byte = 0; byte < 4; ++byte)
        result[byte] = static_cast<char>((input.size() >> (8 * byte)) & 255);
    std::copy(input.begin(), input.end(), result.begin() + 4);
    constexpr std::uint32_t mask = 1000000 / 7;
    for (std::size_t offset = 0; offset < result.size(); ++offset)
    {
        const auto previous = offset < 4
            ? static_cast<unsigned char>(mask >> (offset * 8))
            : static_cast<unsigned char>(result[offset - 4]);
        result[offset] = static_cast<char>(static_cast<unsigned char>(result[offset]) ^ previous);
    }
    return result;
}
}

int main()
try
{
    for (std::int32_t length = 0; length <= 4096; ++length)
    {
        std::vector<char> input(length);
        for (std::int32_t i = 0; i < length; ++i)
            input[i] = static_cast<char>((i * 131 + length) & 255);
        const auto expected = legacy_bytes(input);
        // Deliberately unaligned destinations, with guards on both sides.
        std::vector<char> output(expected.size() + 2, '!');
        auto capacity = static_cast<std::int32_t>(expected.size());
        const char empty = 0;
        check(Encrypt(length, input.empty() ? &empty : input.data(), capacity, output.data() + 1), "encrypt");
        check(capacity == expected.size(), "encoded length");
        check(std::equal(expected.begin(), expected.end(), output.begin() + 1), "legacy byte parity");
        check(output.front() == '!' && output.back() == '!', "encrypt guards");

        auto decoded = output;
        auto decoded_length = capacity;
        check(Decrypt(capacity, output.data() + 1, decoded_length, decoded.data() + 1), "decrypt");
        check(decoded_length == length, "decoded length");
        check(std::equal(input.begin(), input.end(), decoded.begin() + 1), "round trip");
        check(decoded.front() == '!' && decoded.back() == '!', "decrypt guards");
        check(std::all_of(decoded.begin() + 1 + length, decoded.end() - 1, [](char c) { return c == 0; }), "clear remaining capacity");

        auto alias = output;
        decoded_length = capacity;
        check(Decrypt(capacity, alias.data() + 1, decoded_length, alias.data() + 1), "in-place decrypt");
        check(std::equal(input.begin(), input.end(), alias.begin() + 1), "in-place contents");
        decoded_length = capacity;
        check(Encrypt(length, alias.data() + 1, decoded_length, alias.data() + 1), "in-place encrypt");
        check(alias == output, "in-place byte parity");

        auto short_capacity = capacity - 1;
        const auto before = output;
        check(!Encrypt(length, input.empty() ? &empty : input.data(), short_capacity, output.data() + 1), "reject short encrypt buffer");
        check(short_capacity == capacity - 1 && output == before, "encrypt failure preserves output");
        check(!Decrypt(capacity, expected.data(), short_capacity, output.data() + 1), "reject short decrypt buffer");
        check(short_capacity == capacity - 1 && output == before, "decrypt failure preserves output");
    }

    std::array<char, 32> output;
    output.fill('?');
    const auto unchanged = output;
    const std::array<char, 8> malformed{};
    for (std::int32_t length : {-4, -1, 0, 1, 2, 3, 5, 6, 7, 8})
    {
        std::int32_t capacity = output.size();
        check(!Decrypt(length, malformed.data(), capacity, output.data()), "reject invalid data");
        check(output == unchanged && capacity == output.size(), "invalid data preserves output");
    }
    for (std::int32_t length : {-1, std::numeric_limits<std::int32_t>::max()})
    {
        std::int32_t capacity = output.size();
        check(!Encrypt(length, malformed.data(), capacity, output.data()), "reject invalid encryption length");
        check(output == unchanged && capacity == output.size(), "invalid length preserves output");
    }
    std::int32_t capacity = output.size();
    check(!Encrypt(0, nullptr, capacity, output.data()), "null encrypt input");
    check(!Encrypt(0, malformed.data(), capacity, nullptr), "null encrypt output");
    check(!Decrypt(4, nullptr, capacity, output.data()), "null decrypt input");
    check(!Decrypt(4, malformed.data(), capacity, nullptr), "null decrypt output");
    capacity = -1;
    check(!Encrypt(0, malformed.data(), capacity, output.data()), "negative encrypt capacity");
    check(!Decrypt(4, malformed.data(), capacity, output.data()), "negative decrypt capacity");
    std::cout << checks << " GM file-transform checks passed\n";
    return 0;
}
catch (const std::exception& error)
{
    std::cerr << error.what() << '\n';
    return 1;
}
