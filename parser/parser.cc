#include "parser.hh"

#include <cstddef>
#include <cstdint>

static size_t read_prefix(std::span<std::byte> data, size_t it, size_t length)
{
    uint64_t value = 0;
    for (size_t i = 0; i < length; i++)
    {
        value = (value << 8) | std::to_integer<uint64_t>(data[it + i]);
    }
    return value;
}

void Parser::parse(std::span<std::byte> data, Consumer consumer)
{
    // read first byte, then switch case;
    // read depending on the case;
    // call consumer function
    // move it to length + 2 (2 bytes for size of message)
    // repeat until greater than end of span.
    size_t it = 0;
    while (it + 2 <= data.size())
    {
        size_t len_message = read_prefix(data, it, 2);
        if (it + 2 + len_message > data.size())
            break;

        consumer.on_message(data.subspan(it + 2, len_message));

        it += len_message + 2;
    }
}
