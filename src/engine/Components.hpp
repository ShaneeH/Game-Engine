#pragma once

#include <SDL3/SDL.h>

// Generic components the engine systems understand.

struct Transform
{
    float x = 0.0f;
    float y = 0.0f;
};

struct Velocity
{
    float x = 0.0f;
    float y = 0.0f;
};

enum class CollisionResponse
{
    None,   // passes through solids
    Slide,  // stops against solids, keeps moving on the other axis
    Bounce  // reverses direction when it hits a solid or its bounds
};

// Axis aligned box, positioned by the entity's Transform.
struct Collider
{
    float width = 0.0f;
    float height = 0.0f;
    CollisionResponse response = CollisionResponse::None;
};

// Tag for things moving colliders can't pass through.
struct Solid {};

// Keeps a moving entity inside an area.
struct Bounds
{
    SDL_FRect area{};
};

// A filled rectangle the size of the entity's Collider. Lower layers draw first.
struct Renderable
{
    SDL_Color color{ 255, 255, 255, 255 };
    int layer = 0;
    SDL_Color flashColor{ 255, 255, 255, 255 };
    float flashTime = 0.0f;
};

inline SDL_FRect GetRect(const Transform& transform, const Collider& collider)
{
    return { transform.x, transform.y, collider.width, collider.height };
}
