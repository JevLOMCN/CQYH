#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string_view>
#include <thread>
#include <windows.h>

int wmain(int argc, wchar_t** argv)
{
    if (argc < 2) return 2;
    if (std::wstring_view{argv[1]} == L"exit") return 17;
    if (argc < 3) return 2;
    std::ofstream file(std::filesystem::path{argv[2]}, std::ios::binary);
    if (std::wstring_view{argv[1]} == L"hold")
    {
        file << GetCurrentProcessId(); file.close();
        std::this_thread::sleep_for(std::chrono::seconds(30));
        return 0;
    }
    for (int index = 3; index < argc; ++index)
    {
        const std::wstring_view value{argv[index]};
        const auto length = static_cast<std::uint32_t>(value.size());
        file.write(reinterpret_cast<const char*>(&length), sizeof(length));
        file.write(reinterpret_cast<const char*>(value.data()), length * sizeof(wchar_t));
    }
    return file ? 0 : 3;
}
