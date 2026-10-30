#include "account_logout.h"

#include <iostream>
#include <string_view>

namespace
{
template<class Character>
int run(int argc, Character** argv)
{
    constexpr auto usage = "Usage: account_logout [--input directory]\n";
    try
    {
        std::filesystem::path root{"../account_logout"};
        if (argc == 2 && (std::filesystem::path{argv[1]} == "--help" || std::filesystem::path{argv[1]} == "-h"))
        {
            std::cout << usage;
            return std::cout ? 0 : 1;
        }
        if (argc == 3 && std::filesystem::path{argv[1]} == "--input") root = argv[2];
        else if (argc != 1) { std::cerr << usage; return 1; }
        const auto report = account_logout::analyze(root);
        if (!report) { std::cerr << report.error() << '\n'; return 1; }
        std::cout << report->text;
        return std::cout ? 0 : 1;
    }
    catch (const std::exception& error)
    {
        std::cerr << "account logout analysis failed: " << error.what() << '\n';
        return 1;
    }
}
}

#if defined(_WIN32)
int wmain(int argc, wchar_t** argv) { return run(argc, argv); }
#else
int main(int argc, char** argv) { return run(argc, argv); }
#endif
