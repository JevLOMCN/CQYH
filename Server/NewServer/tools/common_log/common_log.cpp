#include "stdafx.h"
#include "common_log.h"

#include <fstream>
#include <limits>
#include <string>

bool load_stream(std::istream& input, log_context* context)
{
    if (!context || !context->field_func || !context->line_func || !context->end_func)
        return false;
    int line_index = 0;
    std::string line;
    while (std::getline(input, line))
    {
        if (line.ends_with('\r'))
            line.pop_back();
        if (line.find('\0') != line.npos)
            return false;
        int field_index = 0;
        std::size_t begin = 0;
        for (;;)
        {
            const auto end = line.find('\t', begin);
            if (end != line.npos)
                line[end] = '\0';
            if (!context->field_func(field_index, line_index, line.data() + begin, context))
                return false;
            if (end == line.npos)
                break;
            if (field_index == std::numeric_limits<int>::max())
                return false;
            ++field_index;
            begin = end + 1;
        }
        if (!context->line_func(line_index, context) || line_index == std::numeric_limits<int>::max())
            return false;
        ++line_index;
    }
    return input.eof() && !input.bad() && context->end_func(context);
}

bool load_file(const char* file_name, log_context* context)
{
    if (!file_name || !context || !context->field_func || !context->line_func || !context->end_func)
        return false;
    std::ifstream file(file_name, std::ios::binary);
    return file && load_stream(file, context);
}
