#pragma once

#include "Entity.hpp"

#include <cassert>
#include <cstdint>
#include <utility>
#include <vector>

// Type erased base so the registry can strip every component from a destroyed entity.
class IComponentPool
{
public:
    virtual ~IComponentPool() = default;
    virtual void Remove(std::uint32_t index) = 0;
    virtual bool Has(std::uint32_t index) const = 0;
    virtual std::size_t Size() const = 0;
    virtual const std::vector<std::uint32_t>& Indices() const = 0;
};

// Sparse set: components are stored packed in a dense array for fast iteration,
// and the sparse array maps an entity index to its slot in the dense array.
template <typename T>
class ComponentPool final : public IComponentPool
{
public:
    template <typename... Args>
    T& Add(std::uint32_t index, Args&&... args)
    {
        if (Has(index))
        {
            T& existing = components[sparse[index]];
            existing = T{ std::forward<Args>(args)... };
            return existing;
        }

        if (index >= sparse.size()) sparse.resize(index + 1, empty);
        sparse[index] = static_cast<std::uint32_t>(dense.size());
        dense.push_back(index);
        components.push_back(T{ std::forward<Args>(args)... });
        return components.back();
    }

    void Remove(std::uint32_t index) override
    {
        if (!Has(index)) return;

        // Swap the removed slot with the last one to keep the arrays packed.
        std::uint32_t slot = sparse[index];
        std::uint32_t lastIndex = dense.back();
        dense[slot] = lastIndex;
        components[slot] = std::move(components.back());
        sparse[lastIndex] = slot;

        dense.pop_back();
        components.pop_back();
        sparse[index] = empty;
    }

    bool Has(std::uint32_t index) const override
    {
        return index < sparse.size() && sparse[index] != empty;
    }

    T& Get(std::uint32_t index)
    {
        assert(Has(index));
        return components[sparse[index]];
    }

    const T& Get(std::uint32_t index) const
    {
        assert(Has(index));
        return components[sparse[index]];
    }

    std::size_t Size() const override { return dense.size(); }
    const std::vector<std::uint32_t>& Indices() const override { return dense; }

private:
    static constexpr std::uint32_t empty = 0xFFFFFFFFu;

    std::vector<std::uint32_t> sparse;
    std::vector<std::uint32_t> dense;
    std::vector<T> components;
};
