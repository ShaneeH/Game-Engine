#include "Systems.hpp"
#include "Collision.hpp"
#include "Components.hpp"

#include <algorithm>
#include <vector>

namespace
{
    struct SolidBox
    {
        Entity entity;
        SDL_FRect rect;
    };

    // Returns true when the axis was blocked.
    bool MoveAxis(float& position, float& velocity, float deltaTime, bool horizontal,
        Transform& transform, const Collider& collider, const Bounds* bounds,
        const std::vector<SolidBox>& solids, Entity self)
    {
        if (velocity == 0.0f) return false;

        float oldPosition = position;
        position += velocity * deltaTime;
        bool blocked = false;

        if (collider.response == CollisionResponse::Slide)
        {
            float size = horizontal ? collider.width : collider.height;
            for (const SolidBox& solid : solids)
            {
                if (solid.entity == self || !IsColliding(GetRect(transform, collider), solid.rect)) continue;
                float solidStart = horizontal ? solid.rect.x : solid.rect.y;
                float solidSize = horizontal ? solid.rect.w : solid.rect.h;
                position = velocity > 0.0f ? solidStart - size : solidStart + solidSize;
                blocked = true;
            }
        }

        if (bounds)
        {
            float size = horizontal ? collider.width : collider.height;
            float minimum = horizontal ? bounds->area.x : bounds->area.y;
            float maximum = minimum + (horizontal ? bounds->area.w : bounds->area.h) - size;
            if (position < minimum || position > maximum)
            {
                position = std::clamp(position, minimum, maximum);
                if (collider.response == CollisionResponse::Bounce) velocity = -velocity;
                blocked = true;
            }
        }

        if (collider.response == CollisionResponse::Bounce)
        {
            for (const SolidBox& solid : solids)
            {
                if (solid.entity != self && IsColliding(GetRect(transform, collider), solid.rect))
                {
                    position = oldPosition;
                    velocity = -velocity;
                    return true;
                }
            }
        }
        return blocked;
    }
}

void MovementSystem(Registry& registry, float deltaTime)
{
    std::vector<SolidBox> solids;
    registry.View<Solid, Transform, Collider>([&](Entity entity, Solid&, Transform& transform, Collider& collider)
    {
        solids.push_back({ entity, GetRect(transform, collider) });
    });

    registry.View<Transform, Velocity>([&](Entity entity, Transform& transform, Velocity& velocity)
    {
        const Collider* collider = registry.TryGet<Collider>(entity);
        if (!collider)
        {
            transform.x += velocity.x * deltaTime;
            transform.y += velocity.y * deltaTime;
            return;
        }

        const Bounds* bounds = registry.TryGet<Bounds>(entity);
        MoveAxis(transform.x, velocity.x, deltaTime, true, transform, *collider, bounds, solids, entity);
        MoveAxis(transform.y, velocity.y, deltaTime, false, transform, *collider, bounds, solids, entity);
    });
}

void FlashSystem(Registry& registry, float deltaTime)
{
    registry.View<Renderable>([&](Entity, Renderable& renderable)
    {
        renderable.flashTime = std::max(0.0f, renderable.flashTime - deltaTime);
    });
}

void RenderSystem(Registry& registry, SDL_Renderer* renderer)
{
    struct DrawItem
    {
        int layer;
        SDL_Color color;
        SDL_FRect rect;
    };

    std::vector<DrawItem> items;
    registry.View<Renderable, Transform, Collider>([&](Entity, Renderable& renderable, Transform& transform, Collider& collider)
    {
        SDL_Color color = renderable.flashTime > 0.0f ? renderable.flashColor : renderable.color;
        items.push_back({ renderable.layer, color, GetRect(transform, collider) });
    });

    std::stable_sort(items.begin(), items.end(),
        [](const DrawItem& a, const DrawItem& b) { return a.layer < b.layer; });

    for (const DrawItem& item : items)
    {
        SDL_SetRenderDrawColor(renderer, item.color.r, item.color.g, item.color.b, item.color.a);
        SDL_RenderFillRect(renderer, &item.rect);
    }
}
