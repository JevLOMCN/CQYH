#include "base.h"

#include <bit>
#include <cstddef>
#include <cstring>
#include <span>
#include <vector>

namespace
{
constexpr std::uint32_t mask = 1000000 / 7;
constexpr std::size_t word_size = sizeof(std::uint32_t);

std::uint32_t read_word(std::span<const char> data, std::size_t offset)
{
    std::uint32_t value;
    std::memcpy(&value, data.data() + offset, word_size);
    if constexpr (std::endian::native == std::endian::big)
        value = std::byteswap(value);
    return value;
}

void write_word(std::span<char> data, std::size_t offset, std::uint32_t value)
{
    if constexpr (std::endian::native == std::endian::big)
        value = std::byteswap(value);
    std::memcpy(data.data() + offset, &value, word_size);
}
}

bool Encrypt(std::int32_t input_length, const char* input,
             std::int32_t& output_length, char* output)
{
    if (!input || !output || input_length < 0 || output_length < 0)
        return false;

    // Preserve the little-endian length, zero padding and chained XOR format.
    const auto size = (static_cast<std::uint64_t>(input_length) + 7) & ~std::uint64_t{3};
    if (size > static_cast<std::uint64_t>(output_length))
        return false;

    std::vector<char> encoded(static_cast<std::size_t>(size), 0);
    write_word(encoded, 0, static_cast<std::uint32_t>(input_length));
    std::memcpy(encoded.data() + word_size, input, input_length);
    auto previous = mask;
    for (std::size_t offset = 0; offset < encoded.size(); offset += word_size)
    {
        previous ^= read_word(encoded, offset);
        write_word(encoded, offset, previous);
    }

    // Stage first so aliased input/output is valid and failures publish nothing.
    std::memset(output, 0, output_length);
    std::memcpy(output, encoded.data(), encoded.size());
    output_length = static_cast<std::int32_t>(encoded.size());
    return true;
}

bool Decrypt(std::int32_t input_length, const char* input,
             std::int32_t& output_length, char* output)
{
    if (!input || !output || input_length < static_cast<std::int32_t>(word_size) ||
        input_length % word_size != 0 || output_length < input_length)
        return false;

    const std::span<const char> source(input, static_cast<std::size_t>(input_length));
    const auto decoded_length = read_word(source, 0) ^ mask;
    const auto expected_size = (static_cast<std::uint64_t>(decoded_length) + 7) & ~std::uint64_t{3};
    if (expected_size != static_cast<std::uint64_t>(input_length))
        return false;

    std::vector<char> decoded(source.begin(), source.end());
    for (std::size_t offset = decoded.size() - word_size; offset > 0; offset -= word_size)
        write_word(decoded, offset, read_word(source, offset) ^ read_word(source, offset - word_size));

    std::memset(output, 0, output_length);
    std::memcpy(output, decoded.data() + word_size, decoded_length);
    output_length = static_cast<std::int32_t>(decoded_length);
    return true;
}
