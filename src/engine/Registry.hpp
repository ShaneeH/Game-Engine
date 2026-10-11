#pragma once

#include "ComponentPool.hpp"
#include "Entity.hpp"

#include <cassert>
#include <cstddef>
#include <memory>
#include <tuple>
#include <vector>

// Owns every entity and component pool.
class Registry
{
public:
    Entity Create()
    {
        std::uint32_t index;
        if (!freeIndices.empty())
        {
            index = freeIndices.back();
            freeIndices.pop_back();
        }
        else
        {
            index = static_cast<std::uint32_t>(generations.size());
            generations.push_back(0);
            living.push_back(false);
        }
        living[index] = true;
        alive++;
        return { index, generations[index] };
    }

    void Destroy(Entity entity)
    {
        if (!IsAlive(entity)) return;

        for (auto& pool : pools)
        {
            if (pool) pool->Remove(entity.index);
        }
        generations[entity.index]++;
        living[entity.index] = false;
        freeIndices.push_back(entity.index);
        alive--;
    }

    // Destroys every entity. Pools stay allocated so they can be reused.
    void Clear()
    {
        for (std::uint32_t i = 0; i < generations.size(); i++)
        {
            Entity entity{ i, generations[i] };
            if (IsAlive(entity)) Destroy(entity);
        }
    }

    bool IsAlive(Entity entity) const
    {
        return entity.index < generations.size()
            && generations[entity.index] == entity.generation
            && living[entity.index];
    }

    std::size_t AliveCount() const { return alive; }

    template <typename T, typename... Args>
    T& Add(Entity entity, Args&&... args)
    {
        assert(IsAlive(entity));
        return Pool<T>().Add(entity.index, std::forward<Args>(args)...);
    }

    template <typename T>
    void Remove(Entity entity)
    {
        if (IsAlive(entity)) Pool<T>().Remove(entity.index);
    }

    template <typename... Ts>
    bool Has(Entity entity) const
    {
        return IsAlive(entity) && (HasIndex<Ts>(entity.index) && ...);
    }

    template <typename T>
    T& Get(Entity entity)
    {
        assert(Has<T>(entity));
        return Pool<T>().Get(entity.index);
    }

    template <typename T>
    const T& Get(Entity entity) const
    {
        assert(Has<T>(entity));
        return PoolIfExists<T>()->Get(entity.index);
    }

    template <typename T>
    T* TryGet(Entity entity)
    {
        return Has<T>(entity) ? &Pool<T>().Get(entity.index) : nullptr;
    }

    // Calls func(entity, components...) for every entity that has all of Ts.
    // Iterates the smallest pool and checks the others. Don't add or remove
    // components of the viewed types inside the callback.
    template <typename... Ts, typename Func>
    void View(Func&& func)
    {
        static_assert(sizeof...(Ts) > 0, "View needs at least one component type");

        const IComponentPool* smallest = nullptr;
        for (const IComponentPool* pool : { static_cast<const IComponentPool*>(&Pool<Ts>())... })
        {
            if (!smallest || pool->Size() < smallest->Size()) smallest = pool;
        }

        // Copy the indices so destroying the current entity inside the callback is safe.
        std::vector<std::uint32_t> indices = smallest->Indices();
        for (std::uint32_t index : indices)
        {
            if (!(HasIndex<Ts>(index) && ...)) continue;
            func(Entity{ index, generations[index] }, Pool<Ts>().Get(index)...);
        }
    }

    // Returns the first entity with all of Ts, or NullEntity.
    template <typename... Ts>
    Entity Find()
    {
        Entity found = NullEntity;
        View<Ts...>([&](Entity entity, Ts&...) { if (!found.IsValid()) found = entity; });
        return found;
    }

private:
    template <typename T>
    static std::size_t TypeId()
    {
        static const std::size_t id = nextTypeId++;
        return id;
    }

    template <typename T>
    ComponentPool<T>& Pool()
    {
        std::size_t id = TypeId<T>();
        if (id >= pools.size()) pools.resize(id + 1);
        if (!pools[id]) pools[id] = std::make_unique<ComponentPool<T>>();
        return static_cast<ComponentPool<T>&>(*pools[id]);
    }

    template <typename T>
    const ComponentPool<T>* PoolIfExists() const
    {
        std::size_t id = TypeId<T>();
        if (id >= pools.size() || !pools[id]) return nullptr;
        return static_cast<const ComponentPool<T>*>(pools[id].get());
    }

    template <typename T>
    bool HasIndex(std::uint32_t index) const
    {
        const ComponentPool<T>* pool = PoolIfExists<T>();
        return pool && pool->Has(index);
    }

    inline static std::size_t nextTypeId = 0;

    std::vector<std::uint32_t> generations;
    std::vector<std::uint32_t> freeIndices;
    std::vector<bool> living;
    std::vector<std::unique_ptr<IComponentPool>> pools;
    std::size_t alive = 0;
};
