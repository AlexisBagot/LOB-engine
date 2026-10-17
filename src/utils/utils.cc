#include "utils.hh"

#include <iostream>

void utils_print_help()
{
    std::cout << "**LOB-ENGINE HELP**\n\n";
    std::cout << "Available parameters:\n";
    std::cout << "\t-f : allows user to specify file to read data from.\nfile "
                 "has to be in Nasdaq TotalView ITCH 5.0 format.\n ";
}
