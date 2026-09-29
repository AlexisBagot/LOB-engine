#include <span>

#include "../consumer/consumer.hh"

class Parser
{
public:
    void parse(std::span<const std::byte> data, Consumer &consumer);
};
