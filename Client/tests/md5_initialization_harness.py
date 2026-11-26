"""Production MD5 copies: initialized state, result ownership and digest vectors."""
import re


def fixtures(client, source, function):
    def clean(text):
        text = re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)
        return re.sub(r"(?m)^\s*#(?:include|pragma)[^\n]*", "", text)

    parts = ["#include <iostream>\n#include <fstream>\n#include <sstream>\n#include <algorithm>\n"]
    for index, folder in enumerate(["FixClient", "PatchMPQ", "pakpatcher", "vigenerator", "FilePatch"]):
        path = client / "client/PatchTools" / folder
        parts.append("namespace Md5%d {\n" % index)
        parts.append(clean(source(path / "md5.h")).replace("private:", "public:"))
        parts.append(clean(source(path / "md5.cpp")))
        parts.append(r'''
void Run() {
    std::ostringstream errors;
    auto* old_errors = std::cerr.rdbuf(errors.rdbuf());
    for (unsigned char pattern : {0xa5, 0x5a}) {
        alignas(MD5) unsigned char storage[sizeof(MD5)];
        ::memset(storage, pattern, sizeof(storage));
        MD5* digest = new(storage) MD5;
        assert(!digest->finalized && !digest->count[0] && !digest->count[1]);
        for (auto byte : digest->buffer) assert(!byte);
        for (auto byte : digest->digest) assert(!byte);
        unsigned char* raw = digest->raw_digest();
        for (int i = 0; i < 16; ++i) assert(!raw[i]);
        delete[] raw;
        char* hex = digest->hex_digest();
        for (int i = 0; i < 33; ++i) assert(!hex[i]);
        delete[] hex;
        digest->~MD5();
    }
    std::cerr.rdbuf(old_errors);
    for (auto sample : {
            std::make_pair("", "d41d8cd98f00b204e9800998ecf8427e"),
            std::make_pair("a", "0cc175b9c0f1b6a831c399e269772661"),
            std::make_pair("abc", "900150983cd24fb0d6963f7d28e17f72"),
            std::make_pair("message digest", "f96b697d7cb7938d525a2f31aaf161d0")}) {
        std::istringstream input(sample.first);
        MD5 digest(input);
        char* text = digest.hex_digest();
        assert(!strcmp(text, sample.second));
        delete[] text;
        unsigned char* raw = digest.raw_digest();
        assert(!memcmp(raw, digest.digest, 16));
        delete[] raw;
        for (auto byte : digest.buffer) assert(!byte);
        std::ostringstream formatted;
        formatted << digest;
        assert(formatted.str() == sample.second);
    }
    for (int size : {55, 56, 63, 64, 65, 1023, 1024, 1025, 10000}) {
        std::string bytes(size, 'x');
        MD5 whole, fragmented;
        whole.update(reinterpret_cast<unsigned char*>(&bytes[0]), (unsigned)bytes.size());
        for (char& byte : bytes) fragmented.update(reinterpret_cast<unsigned char*>(&byte), 1);
        whole.finalize(); fragmented.finalize();
        assert(!memcmp(whole.digest, fragmented.digest, 16));
        for (auto byte : whole.buffer) assert(!byte);
        for (auto byte : fragmented.buffer) assert(!byte);
    }
}
}
''')
    parts.append(r'''
namespace HashComparisonTests {
using Utility2::PathString;
using Md52::MD5;
DWORD GetFileSize(const PathString& path) {
    FILE* input = nullptr;
    if (fopen_s(&input, path, "rb")) return (DWORD)-1;
    fseek(input, 0, SEEK_END);
    DWORD result = (DWORD)ftell(input);
    fclose(input);
    return result;
}
''')
    text = clean(source(client / "client/PatchTools/pakpatcher/PakPatcher.inl"))
    start = text.index("bool CompareFile(const PathString& FileName, const PathString& RefFileName)")
    parts.append(function(text[start:], "bool CompareFile("))
    parts.append(function(text, "bool CompareFileWithMD5("))
    parts.append(function(source(client / "client/PatchTools/PatchMPQ/PatchMPQUtil.h"),
                          "bool CompareFileWithMD5(").replace(
                              "bool CompareFileWithMD5(", "bool ComparePatchFileWithMD5(", 1))
    parts.append(r'''
void Run() {
    FILE* file = nullptr;
    assert(!fopen_s(&file, "hash-first.txt", "wb"));
    fputs("abc", file); fclose(file);
    assert(!fopen_s(&file, "hash-second.txt", "wb"));
    fputs("def", file); fclose(file);
    assert(!CompareFile("hash-first.txt", "hash-first.txt"));
    assert(CompareFile("hash-first.txt", "hash-second.txt"));
    assert(CompareFileWithMD5("hash-first.txt", "900150983CD24FB0D6963F7D28E17F72"));
    assert(!CompareFileWithMD5("hash-second.txt", "900150983cd24fb0d6963f7d28e17f72"));
    assert(ComparePatchFileWithMD5("hash-first.txt", "900150983cd24fb0d6963f7d28e17f72"));
}
}
''')
    parts.append("void TestMd5() { HashComparisonTests::Run(); " + " ".join("Md5%d::Run();" % i for i in range(5)) +
                 ' puts("MD5 initialization, ownership and known-vector regressions passed (five copies)"); }')
    return "\n".join(parts)
