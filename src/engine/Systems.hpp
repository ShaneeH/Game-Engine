#pragma once

#include "Registry.hpp"

#include <SDL3/SDL.h>

// Moves every entity with Transform and Velocity, one axis at a time,
// and resolves collisions against Solid entities.
void MovementSystem(Registry& registry, float deltaTime);

// Counts down Renderable flash timers.
void FlashSystem(Registry& registry, float deltaTime);

// Draws every Renderable as a filled rectangle, sorted by layer.
void RenderSystem(Registry& registry, SDL_Renderer* renderer);
