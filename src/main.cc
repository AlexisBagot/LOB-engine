#include <fcntl.h>
#include <iostream>
#include <span>
#include <stdio.h>
#include <string>
#include <string_view>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

#include "consumer/consumer.hh"
#include "parser/parser.hh"
#include "utils/utils.hh"

int main(int argc, char **argv)
{
    std::string filename;
    if (argc > 1)
    {
        std::vector<std::string_view> args(argv + 1, argv + argc);
        if (args[0] == "-h")
        {
            utils_print_help();
            return 0;
        }
        for (int i = 0; i < argc; i++)
        {
            if (args[i] == "-f" && i + 1 < argc)
            {
                filename = args[i + 1];
            }
        }
    }
    else
    {
        utils_print_help();
        return 1;
    }

    // reading logic

    int fd = open(filename.data(), O_RDONLY);
    if (fd == -1)
    {
        std::cerr << "file does not exist / open error\n";
        return 1;
    }

    struct stat st;
    fstat(fd, &st);

    std::vector<std::byte> buf(st.st_size);
    size_t readDataCounter = 0;
    while (readDataCounter < buf.size())
    {
        ssize_t sizeRead = read(fd, buf.data() + readDataCounter,
                                buf.size() - readDataCounter);
        readDataCounter += sizeRead;
        if (sizeRead == 0)
            break;
        else if (sizeRead == -1)
        {
            std::cerr << "reading error!\n";
            return 1;
        }
    }
    close(fd);

    if (readDataCounter != buf.size())
    {
        std::cerr << "Wrong amount of data read: expected " << buf.size()
                  << " bytes, got " << readDataCounter << " bytes.\n";
        return 1;
    }

    std::span<const std::byte> data(buf);

    // parsing logic
    Consumer consumer;
    Parser parser;
    parser.parse(data, consumer);
}
