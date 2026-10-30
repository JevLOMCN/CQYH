#include "stdafx.h"
#include "math_ex.h"
#include "cert_math.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <iterator>

namespace cert
{
void initialize_math_table(MATH_TABLE& table)
{
    // PI and the rounding expression are part of the serialized table's legacy
    // contract. A more precise pi constant would change certification bytes.
    for (auto& row : table.nDirectionTable) std::ranges::fill(row, 0);
    for (int direction = 0; direction < MAX_DIRECTION_COUNT; ++direction)
    {
        const double angle = static_cast<double>(direction) * 2 * PI / MAX_DIRECTION_COUNT;
        table.nSinTable[direction] = static_cast<INT32>(std::floor(std::sin(angle) * SIN_COS_NUMBER + 0.5f));
        table.nCosTable[direction] = static_cast<INT32>(std::floor(std::cos(angle) * SIN_COS_NUMBER + 0.5f));
    }
    for (int direction = 0; direction < MAX_DIRECTION_COUNT / 4; ++direction)
        table.nSinSquareTable[direction] = table.nSinTable[direction] * table.nSinTable[direction];
    for (int value = 0; value < MAX_SQUARE; ++value)
        table.wSqrtTable[value] = static_cast<UINT16>(std::floor(std::sqrt(static_cast<double>(value)) + 0.5f));

    constexpr std::array<INT32, DT_DIRECTION_8_COUNT> adjacentX{1, 1, 0, -1, -1, -1, 0, 1};
    constexpr std::array<INT32, DT_DIRECTION_8_COUNT> adjacentY{0, 1, 1, 1, 0, -1, -1, -1};
    constexpr std::array<std::array<INT32, 6>, DT_DIRECTION_8_COUNT> adjacent{{
        {3, DT_NORTH_EAST, DT_EAST, DT_SOUTH_EAST},
        {5, DT_NORTH_EAST, DT_EAST, DT_SOUTH_EAST, DT_SOUTH, DT_SOUTH_WEST},
        {3, DT_SOUTH_EAST, DT_SOUTH, DT_SOUTH_WEST},
        {5, DT_SOUTH_EAST, DT_SOUTH, DT_SOUTH_WEST, DT_WEST, DT_NORTH_WEST},
        {3, DT_SOUTH_WEST, DT_WEST, DT_NORTH_WEST},
        {5, DT_SOUTH_WEST, DT_WEST, DT_NORTH_WEST, DT_NORTH, DT_NORTH_EAST},
        {3, DT_NORTH_WEST, DT_NORTH, DT_NORTH_EAST},
        {5, DT_NORTH_WEST, DT_NORTH, DT_NORTH_EAST, DT_EAST, DT_SOUTH_EAST}}};
    std::ranges::copy(adjacentX, std::begin(table.nAdjacentX));
    std::ranges::copy(adjacentY, std::begin(table.nAdjacentY));
    for (std::size_t row = 0; row < adjacent.size(); ++row)
        std::ranges::copy(adjacent[row], std::begin(table.nAdjList[row]));
}
}
