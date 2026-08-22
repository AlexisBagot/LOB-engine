#include <span>

#include "../consumer/consumer.hh"

class Parser
{
public:
    void parse(std::span<std::byte> data, Consumer consumer);
};
