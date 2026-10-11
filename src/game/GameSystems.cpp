#include "GameSystems.hpp"
#include "GameComponents.hpp"
#include "../engine/Collision.hpp"
#include "../engine/Components.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <string>

namespace
{
    bool IsSpawnFree(Registry& registry, const SDL_FRect& rect)
    {
        bool free = true;
        registry.View<Transform, Collider>([&](Entity, Transform& transform, Collider& collider)
        {
            if (free && IsColliding(rect, GetRect(transform, collider))) free = false;
        });
        return free;
    }

    void AddScore(Registry& registry, GameState& state, Entity player, int points)
    {
        state.score += points;
        state.highScore = std::max(state.score, state.highScore);
        if (Renderable* renderable = registry.TryGet<Renderable>(player))
        {
            renderable->flashTime = GameState::flashLength;
        }
    }
}

bool FindSpawn(Registry& registry, const World& world, SDL_FRect& rect)
{
    int width = static_cast<int>(world.width);
    int height = static_cast<int>(world.height);

    for (int attempt = 0; attempt < 100; attempt++)
    {
        SDL_FRect candidate = rect;
        candidate.x = 30.0f + static_cast<float>(std::rand() % (width - 60 - static_cast<int>(rect.w)));
        candidate.y = 120.0f + static_cast<float>(std::rand() % (height - 150 - static_cast<int>(rect.h)));
        if (IsSpawnFree(registry, candidate))
        {
            rect = candidate;
            return true;
        }
    }

    for (float y = 120.0f; y + rect.h < world.height - 30.0f; y += 40.0f)
    {
        for (float x = 30.0f; x + rect.w < world.width - 30.0f; x += 40.0f)
        {
            SDL_FRect candidate = { x, y, rect.w, rect.h };
            if (IsSpawnFree(registry, candidate))
            {
                rect = candidate;
                return true;
            }
        }
    }
    return false;
}

void InputSystem(Registry& registry, const bool* keys)
{
    float directionX = 0.0f;
    float directionY = 0.0f;

    if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]) directionY -= 1.0f;
    if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN]) directionY += 1.0f;
    if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT]) directionX -= 1.0f;
    if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) directionX += 1.0f;

    float length = std::sqrt(directionX * directionX + directionY * directionY);
    if (length > 0.0f)
    {
        directionX /= length;
        directionY /= length;
    }

    bool sprinting = keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT];

    registry.View<PlayerControl, Velocity>([&](Entity, PlayerControl& control, Velocity& velocity)
    {
        float speed = control.moveSpeed * (sprinting ? control.sprintMultiplier : 1.0f);
        velocity.x = directionX * speed;
        velocity.y = directionY * speed;
    });
}

void PickupSystem(Registry& registry, GameState& state, const World& world)
{
    Entity player = registry.Find<PlayerControl, Transform, Collider>();
    if (!player.IsValid()) return;

    auto playerRect = [&] { return GetRect(registry.Get<Transform>(player), registry.Get<Collider>(player)); };

    registry.View<Target, Transform, Collider, Velocity>(
        [&](Entity, Target& target, Transform& transform, Collider& collider, Velocity& velocity)
    {
        SDL_FRect next = GetRect(transform, collider);
        if (!IsColliding(playerRect(), next) || !FindSpawn(registry, world, next)) return;

        transform.x = next.x;
        transform.y = next.y;
        state.targetsCaught++;
        AddScore(registry, state, player, target.points);
        float speed = std::min(target.maxSpeed, target.baseSpeed + static_cast<float>(state.targetsCaught) * target.speedStep);
        velocity.y = velocity.y < 0.0f ? -speed : speed;
    });

    registry.View<Pickup, Transform, Collider>([&](Entity, Pickup& pickup, Transform& transform, Collider& collider)
    {
        SDL_FRect next = GetRect(transform, collider);
        if (!IsColliding(playerRect(), next) || !FindSpawn(registry, world, next)) return;

        transform.x = next.x;
        transform.y = next.y;
        AddScore(registry, state, player, pickup.points);
    });
}

void HudSystem(SDL_Renderer* renderer, const GameState& state)
{
    SDL_SetRenderScale(renderer, 2.0f, 2.0f);
    SDL_SetRenderDrawColor(renderer, 240, 240, 245, 255);
    std::string hud = "Score: " + std::to_string(state.score) + "    Best: " + std::to_string(state.highScore)
        + "    Time: " + std::to_string(static_cast<int>(std::ceil(state.timeLeft)));
    SDL_RenderDebugText(renderer, 12.0f, 10.0f, hud.c_str());
    SDL_RenderDebugText(renderer, 12.0f, 25.0f, "WASD / Arrows: move   Shift: sprint   P: pause   R: restart");
    SDL_RenderDebugText(renderer, 12.0f, 40.0f, "Blue: 10 points   Gold: 1 point   F11: fullscreen   Esc: quit");

    if (state.paused || state.gameOver)
    {
        SDL_FRect panel = { 220.0f, 260.0f, 360.0f, 75.0f };
        SDL_SetRenderDrawColor(renderer, 35, 35, 55, 255);
        SDL_RenderFillRect(renderer, &panel);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDebugText(renderer, 250.0f, 280.0f,
            state.gameOver ? "TIME UP! Press R to play again" : "PAUSED. Press P to continue");
        SDL_RenderDebugText(renderer, 250.0f, 300.0f, hud.c_str());
    }
    SDL_SetRenderScale(renderer, 1.0f, 1.0f);
}
