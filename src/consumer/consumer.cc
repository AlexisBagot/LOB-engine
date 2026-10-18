#include "consumer.hh"

#include <iostream>

void Consumer::on_message(std::span<const std::byte> data)
{
    ++messages_codes[std::to_integer<uint8_t>(data[0])];
}

void Consumer::report() const
{
    uint64_t totalConsumed = 0;
    for (size_t i = 0; i < 256; i++)
    {
        if (messages_codes[i] == 0)
            continue;
        std::printf("%c (0x%02zx) : %lu\n", (char)i, i, messages_codes[i]);
        totalConsumed += messages_codes[i];
    }

    std::cout << "total consumed : " << totalConsumed << " messages.\n";
}
