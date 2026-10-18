#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

class Consumer
{
private:
    std::array<uint64_t, 256> messages_codes{};

public:
    void on_message(std::span<const std::byte> data);
    void report() const;
    const std::array<uint64_t, 256> &codes() const
    {
        return messages_codes;
    }
};
