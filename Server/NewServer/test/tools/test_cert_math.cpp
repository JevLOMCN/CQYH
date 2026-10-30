#include <winsock2.h>
#include <windows.h>
#include "std_type.h"
#include "process_error.h"
#include "math_ex.h"
#include "cert_math.h"
#include <cstdio>
#include <cstring>
#include <memory>

namespace
{
#include "legacy_cert_math.inl"
}

int main()
{
    int checks = 0, failures = 0;
    const auto check = [&](bool value, const char* message) {
        ++checks;
        if (!value) { ++failures; std::fprintf(stderr, "FAIL: %s\n", message); }
    };
    const auto before = std::make_unique<MATH_TABLE>();
    const auto after = std::make_unique<MATH_TABLE>();
    std::memset(after.get(), 0x5a, sizeof(*after));
    legacy_cert_math(*before);
    cert::initialize_math_table(*after);
    check(std::memcmp(before.get(), after.get(), sizeof(*before)) == 0,
        "all serialized bytes match the legacy generator, including previously nonzero storage");
    check(after->nSinTable[0] == 0 && after->nCosTable[0] == SIN_COS_NUMBER,
        "zero direction retains sine/cosine scale");
    check(after->nSinTable[MAX_DIRECTION_COUNT / 4] == SIN_COS_NUMBER && after->nCosTable[MAX_DIRECTION_COUNT / 4] == 0,
        "quarter turn retains legacy rounding");
    check(after->wSqrtTable[0] == 0 && after->wSqrtTable[1] == 1 && after->wSqrtTable[2] == 1 &&
        after->wSqrtTable[3] == 2 && after->wSqrtTable[MAX_SQUARE - 1] == 256,
        "sqrt table uses rounded roots rather than integer floor");
    for (int direction = 0; direction < MAX_DIRECTION_COUNT; ++direction)
        check(after->nSinTable[direction] == before->nSinTable[direction] && after->nCosTable[direction] == before->nCosTable[direction],
            "each trigonometric entry is unchanged");
    for (int direction = 0; direction < MAX_DIRECTION_COUNT / 4; ++direction)
        check(after->nSinSquareTable[direction] == before->nSinSquareTable[direction], "each squared entry is unchanged");
    for (int value = 0; value < MAX_SQUARE; ++value)
        check(after->wSqrtTable[value] == before->wSqrtTable[value], "each rounded sqrt entry is unchanged");
    cert::initialize_math_table(*after);
    check(std::memcmp(before.get(), after.get(), sizeof(*before)) == 0, "repeated generation is stable");
    std::printf("cert math: %d checks, %d failures\n", checks, failures);
    return failures != 0;
}
