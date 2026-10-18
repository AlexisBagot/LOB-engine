#pragma once
#include <span>

#include "../consumer/consumer.hh"

class Parser
{
public:
    size_t parse(std::span<const std::byte> data, Consumer &consumer);
};
