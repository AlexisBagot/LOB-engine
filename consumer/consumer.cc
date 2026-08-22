#include "consumer.hh"

void Consumer::on_message(std::span<std::byte> data)
{
    ++messages_codes[std::to_integer<uint8_t>(data[0])];
}
