#include "stdafx.h"
#include "anticheat_log.h"

#include <cstdlib>
#include <exception>
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc > 2)
    {
        std::cerr << "Usage: common_log [anticheat-log]\n";
        return EXIT_FAILURE;
    }
    try
    {
		if (load_file(argc == 2 ? argv[1] : "d:\\log\\anticheat111.txt", anticheat_init())) //Don't think this should be hardcoded, maybe pull from config file or command line argument -Jev
            return EXIT_SUCCESS;
        std::cerr << "common_log: cannot read or process the log\n";
    }
    catch (const std::exception& error)
    {
        std::cerr << "common_log: " << error.what() << '\n';
    }
    return EXIT_FAILURE;
}
