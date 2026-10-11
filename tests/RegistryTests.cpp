// Headless checks for the ECS registry. No SDL needed.
#include "../src/engine/Registry.hpp"

#include <cstdlib>
#include <iostream>

namespace
{
    int failures = 0;

    void Check(bool condition, const char* message)
    {
        if (!condition)
        {
            std::cerr << "FAILED: " << message << '\n';
            failures++;
        }
    }

    struct Position { float x = 0.0f; float y = 0.0f; };
    struct Speed { float value = 0.0f; };
    struct Tag {};
}

int main()
{
    Registry registry;

    Entity a = registry.Create();
    Entity b = registry.Create();
    Entity c = registry.Create();
    Check(registry.AliveCount() == 3, "three entities alive");

    registry.Add<Position>(a, 1.0f, 2.0f);
    registry.Add<Position>(b, 3.0f, 4.0f);
    registry.Add<Speed>(b, 5.0f);
    registry.Add<Tag>(c);

    Check(registry.Has<Position>(a), "a has Position");
    Check(!registry.Has<Speed>(a), "a has no Speed");
    Check(registry.Has<Position, Speed>(b), "b has Position and Speed");
    Check(registry.Get<Position>(b).y == 4.0f, "Get returns stored value");

    int count = 0;
    registry.View<Position, Speed>([&](Entity entity, Position& position, Speed& speed)
    {
        Check(entity == b, "view only visits b");
        position.x += speed.value;
        count++;
    });
    Check(count == 1, "view visits one entity");
    Check(registry.Get<Position>(b).x == 8.0f, "view writes through references");

    registry.Remove<Position>(a);
    Check(!registry.Has<Position>(a), "Remove strips component");
    Check(registry.Get<Position>(b).x == 8.0f, "swap remove keeps other data intact");

    registry.Destroy(b);
    Check(!registry.IsAlive(b), "destroyed entity is dead");
    Check(registry.TryGet<Speed>(b) == nullptr, "dead entity has no components");

    Entity reused = registry.Create();
    Check(reused.index == b.index, "index is recycled");
    Check(reused != b, "generation differs after reuse");
    Check(!registry.IsAlive(b), "stale handle stays dead");
    Check(!registry.Has<Position>(reused), "recycled entity starts empty");

    Check(registry.Find<Tag>() == c, "Find returns tagged entity");

    // Destroying inside a view must be safe.
    for (int i = 0; i < 10; i++) registry.Add<Tag>(registry.Create());
    registry.View<Tag>([&](Entity entity, Tag&) { registry.Destroy(entity); });
    Check(registry.Find<Tag>() == NullEntity, "all tagged entities destroyed");

    registry.Clear();
    Check(registry.AliveCount() == 0, "Clear destroys everything");

    if (failures == 0) std::cout << "All registry tests passed\n";
    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
