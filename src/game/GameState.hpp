#pragma once

// Round state shared by the game systems.
struct GameState
{
    static constexpr float roundLength = 60.0f;
    static constexpr float flashLength = 0.15f;

    bool paused = false;
    bool gameOver = false;
    float timeLeft = roundLength;
    int score = 0;
    int highScore = 0;
    int targetsCaught = 0;
};
