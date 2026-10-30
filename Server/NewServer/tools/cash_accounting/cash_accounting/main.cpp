#include "stdafx.h"
#include "cash_accounting.h"
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string_view>

int main(int argc, char* argv[])
{
    if (argc == 2 && std::string_view(argv[1]) == "--help")
    {
        std::cout << "Usage: cash_accounting [--help]\nUses the configured database settings and appends money_accounting.\n";
        return EXIT_SUCCESS;
    }
    if (argc != 1)
    {
        std::cerr << "Usage: cash_accounting [--help]\n";
        return EXIT_FAILURE;
    }
    try
    {
        CCashAccounting accounting;
        if (!accounting.Init() || !accounting.MainLoop() || !accounting.UnInit())
        {
            std::cerr << "cash_accounting: report generation failed\n";
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    catch (const std::exception& error)
    {
        std::cerr << "cash_accounting: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
}
