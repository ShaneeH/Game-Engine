#pragma once

#include "GameState.hpp"
#include "../engine/Registry.hpp"

#include <SDL3/SDL.h>

struct World
{
    float width = 0.0f;
    float height = 0.0f;
};

// Turns the keyboard into player velocity. Diagonals are normalized.
void InputSystem(Registry& registry, const bool* keyboardState);

// Handles the player touching targets and pickups.
void PickupSystem(Registry& registry, GameState& state, const World& world);

void HudSystem(SDL_Renderer* renderer, const GameState& state);

// Moves rect to a random spot that doesn't overlap any collider.
bool FindSpawn(Registry& registry, const World& world, SDL_FRect& rect);
