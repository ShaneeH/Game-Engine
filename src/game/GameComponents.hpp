#pragma once

// Components specific to this game.

struct PlayerControl
{
    float moveSpeed = 300.0f;
    float sprintMultiplier = 1.7f;
};

// Gold square. Respawns somewhere free when collected.
struct Pickup
{
    int points = 1;
};

// Blue target. Respawns and speeds up every time it is caught.
struct Target
{
    int points = 10;
    float baseSpeed = 200.0f;
    float speedStep = 20.0f;
    float maxSpeed = 420.0f;
};

struct Wall {};
