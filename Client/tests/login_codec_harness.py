"""Validate the real negotiated-size decoder using real zlib streams."""
import re
import struct
import zlib


def harness(client, game, source, function):
    table = source(client / "Common/message/external_message_size.cpp")
    table = table[table.index("INT32 g_nS2CMessageSize[] = {"):]
    table = table[table.index("{") + 1:table.index("};")]
    table = re.sub(r"//[^\n]*", "", table)
    values = [int(value) for value in re.findall(r"-?\d+", table)][:1023]
    assert len(values) == 1023
    data = struct.pack("<1023i", *values)
    compressed = zlib.compress(data)
    cases = [zlib.compress(data[:-4]), zlib.compress(data + b"\0\0\0\0"),
             compressed[:-1] + bytes([compressed[-1] ^ 1])]
    for invalid in [-2, 1, 65536]:
        changed = values.copy()
        changed[100] = invalid
        cases.append(zlib.compress(struct.pack("<1023i", *changed)))
    include = (client / "client/UE3/Development/External/zlib/Src/zlib.h").as_posix()
    parts = ['#include "' + include + '"', r'''
namespace LoginCodecTests {
INT32 g_nS2CMessageSize[max_message_count] = {};
''', function(source(game / "net/message_handler_Login.cpp"), "static BOOL LoadServerMessageSizes(")]
    parts.append("void Run() {\nconst INT32 expected[] = {" + ",".join(map(str, values)) + "};")
    parts.append("const unsigned char compressed[] = {" + ",".join(map(str, compressed)) + "};")
    parts.append(r'''
    std::fill(g_nS2CMessageSize, g_nS2CMessageSize + max_message_count, 12345);
    const std::vector<INT32> original(g_nS2CMessageSize, g_nS2CMessageSize + max_message_count);
    assert(!LoadServerMessageSizes(nullptr, 100));
    for (UINT32 size = 0; size < sizeof(compressed); ++size) {
        std::vector<char> truncated(compressed, compressed + size);
        assert(!LoadServerMessageSizes(truncated.data(), size));
        assert(std::equal(original.begin(), original.end(), g_nS2CMessageSize));
    }
''')
    for case in cases:
        parts.append("{ const unsigned char malformed[] = {" + ",".join(map(str, case)) + "};\n"
                     "assert(!LoadServerMessageSizes((const char*)malformed, sizeof(malformed)));\n"
                     "assert(std::equal(original.begin(), original.end(), g_nS2CMessageSize)); }")
    parts.append(r'''
    assert(LoadServerMessageSizes((const char*)compressed, sizeof(compressed)));
    assert(std::equal(expected, expected + ARRAY_COUNT(expected), g_nS2CMessageSize));
    assert(g_nS2CMessageSize[max_message_count - 1] == 12345);
    puts("Login compressed-table boundary and rollback regressions passed");
}
}
''')
    return "\n".join(parts)
