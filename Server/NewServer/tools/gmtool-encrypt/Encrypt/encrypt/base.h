#ifndef CQYH_GM_FILE_TRANSFORM_H
#define CQYH_GM_FILE_TRANSFORM_H

#include <cstdint>

// output_length supplies capacity and receives the number of published bytes.
// Invalid input leaves output and output_length unchanged. Allocation failures
// propagate; no output is published before staging succeeds.
bool Encrypt(std::int32_t input_length, const char* input,
             std::int32_t& output_length, char* output);
bool Decrypt(std::int32_t input_length, const char* input,
             std::int32_t& output_length, char* output);

#endif

