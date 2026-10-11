#pragma once

#include <cstdint>

// An entity is just an id. The generation changes every time an index is
// reused, so a stale handle to a destroyed entity is never mistaken for a new one.
struct Entity
{
    std::uint32_t index = invalidIndex;
    std::uint32_t generation = 0;

    static constexpr std::uint32_t invalidIndex = 0xFFFFFFFFu;

    constexpr bool IsValid() const { return index != invalidIndex; }

    friend constexpr bool operator==(Entity a, Entity b)
    {
        return a.index == b.index && a.generation == b.generation;
    }
    friend constexpr bool operator!=(Entity a, Entity b) { return !(a == b); }
};

inline constexpr Entity NullEntity{};
