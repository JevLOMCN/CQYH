#ifndef CQYH_BASELIB_STRING_EX_H
#define CQYH_BASELIB_STRING_EX_H

#include <cstddef>

// Copy/append at most size - 1 bytes and return the resulting stored length.
// Null pointers or zero capacity return zero without changing the destination.
// An unterminated append destination is truncated to size - 1 bytes.
std::size_t strxcpy(char* dst, const char* src, std::size_t size);
std::size_t strxcat(char* dst, const char* src, std::size_t size);

// Replaces the next separator with NUL and advances the cursor.
// No separator (including a NUL separator) leaves the input alone.
char* SplitStr(char*& tszString, char nSep);

#ifdef WIN32
// Existing formatting callers still depend on _snprintf's truncation behavior.
// Migrate those callers before removing this compatibility alias.
#define snprintf _snprintf
#endif

#endif  // CQYH_BASELIB_STRING_EX_H
