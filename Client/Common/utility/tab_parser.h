#ifndef CLIENT_COMMON_TAB_PARSER_H
#define CLIENT_COMMON_TAB_PARSER_H

#include <climits>
#include <cstddef>
#include <vector>

namespace ClientTable
{
    // The caller supplies length + 1 writable characters. Compact decoded
    // cells in place and return a complete, rectangular table of offsets.
    template<class Character>
    bool Parse(Character* data, size_t length, std::vector<size_t>& offsets,
               int& columns, int& rows)
    {
        offsets.clear();
        columns = rows = 0;
        if (!data || length == 0 || length > INT_MAX)
            return false;
        while (length > 0 && data[length - 1] == 0)
            --length;
        if (length == 0)
            return false;

        size_t input = 0;
        size_t output = 0;
        size_t cellStart = 0;
        int rowColumns = 0;
        bool quoted = false;
        bool pendingRow = false;
        while (input < length)
        {
            const Character value = data[input++];
            if (value == 0)
                return false;
            pendingRow = true;
            if (quoted)
            {
                if (value == '"')
                {
                    if (input < length && data[input] == '"')
                    {
                        data[output++] = '"';
                        ++input;
                    }
                    else
                        quoted = false;
                }
                else
                    data[output++] = value;
            }
            else if (value == '"' && output == cellStart)
                quoted = true;
            else if (value == '\t' || value == '\r' || value == '\n')
            {
                if (offsets.size() >= INT_MAX)
                    return false;
                data[output++] = 0;
                offsets.push_back(cellStart);
                cellStart = output;
                ++rowColumns;
                if (value != '\t')
                {
                    if (value == '\r' && input < length && data[input] == '\n')
                        ++input;
                    if (columns == 0)
                        columns = rowColumns;
                    if (columns != rowColumns)
                        return false;
                    ++rows;
                    rowColumns = 0;
                    pendingRow = false;
                }
            }
            else
                data[output++] = value;
        }
        if (quoted)
            return false;
        if (pendingRow)
        {
            if (offsets.size() >= INT_MAX)
                return false;
            data[output] = 0;
            offsets.push_back(cellStart);
            ++rowColumns;
            if (columns == 0)
                columns = rowColumns;
            if (columns != rowColumns)
                return false;
            ++rows;
        }
        data[output] = 0;
        return columns > 0 && rows > 0;
    }
}

#endif
