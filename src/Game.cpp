#include "Game.hpp"
#include "../Collision.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <string>

Game::Game()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        throw std::runtime_error(SDL_GetError());
    }

    window = SDL_CreateWindow("Shane Engine", worldWidth, worldHeight, SDL_WINDOW_RESIZABLE);
    if (window)
    {
        renderer = SDL_CreateRenderer(window, nullptr);
    }

    if (!renderer)
    {
        std::string error = SDL_GetError();
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw std::runtime_error(error);
    }

    SDL_SetRenderLogicalPresentation(renderer, worldWidth, worldHeight,
        SDL_LOGICAL_PRESENTATION_LETTERBOX);

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    walls = {
        { 350.0f, 300.0f, 60.0f, 330.0f },
        { 700.0f, 750.0f, 350.0f, 60.0f },
        { 1150.0f, 250.0f, 60.0f, 350.0f }
    };

    Reset();
    running = true;
    lastFrameTime = SDL_GetTicks();
    std::cout << "Engine Started\n";
}

Game::~Game()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::Run()
{
    while (running)
    {
        Uint64 currentTime = SDL_GetTicks();
        deltaTime = std::min((currentTime - lastFrameTime) / 1000.0f, 0.05f);
        lastFrameTime = currentTime;

        ProcessInput();
        if (!running) break;
        Update();
        Render();
        SDL_Delay(16);
    }
}

void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT) running = false;
        if (event.type == SDL_EVENT_WINDOW_FOCUS_LOST) paused = true;

        if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat)
        {
            if (event.key.key == SDLK_ESCAPE) running = false;
            if (event.key.key == SDLK_P && !gameOver) paused = !paused;
            if (event.key.key == SDLK_R) Reset();
            if (event.key.key == SDLK_F11)
            {
                if (SDL_SetWindowFullscreen(window, !fullscreen))
                {
                    fullscreen = !fullscreen;
                }
            }
        }
    }
}

void Game::Reset()
{
    player.Reset();
    score = 0;
    targetsCaught = 0;
    timeLeft = 60.0f;
    collectFlash = 0.0f;
    paused = false;
    gameOver = false;
    enemyVelocityY = 200.0f;
    enemyRect = { 700.0f, 300.0f, 80.0f, 80.0f };

    coins.clear();
    for (int i = 0; i < 6; i++)
    {
        SDL_FRect coin = { 0.0f, 0.0f, 24.0f, 24.0f };
        if (FindSpawn(coin)) coins.push_back(coin);
    }
}

bool Game::IsSpawnFree(const SDL_FRect& rect) const
{
    if (IsColliding(rect, player.GetRect()) || IsColliding(rect, enemyRect)) return false;

    for (const SDL_FRect& wall : walls)
    {
        if (IsColliding(rect, wall)) return false;
    }
    for (const SDL_FRect& coin : coins)
    {
        if (IsColliding(rect, coin)) return false;
    }
    return true;
}

bool Game::FindSpawn(SDL_FRect& rect)
{
    for (int attempt = 0; attempt < 100; attempt++)
    {
        SDL_FRect candidate = rect;
        candidate.x = 30.0f + static_cast<float>(std::rand() % (worldWidth - 60 - static_cast<int>(rect.w)));
        candidate.y = 120.0f + static_cast<float>(std::rand() % (worldHeight - 150 - static_cast<int>(rect.h)));
        if (IsSpawnFree(candidate))
        {
            rect = candidate;
            return true;
        }
    }

    for (float y = 120.0f; y + rect.h < worldHeight - 30.0f; y += 40.0f)
    {
        for (float x = 30.0f; x + rect.w < worldWidth - 30.0f; x += 40.0f)
        {
            SDL_FRect candidate = { x, y, rect.w, rect.h };
            if (IsSpawnFree(candidate))
            {
                rect = candidate;
                return true;
            }
        }
    }
    return false;
}

void Game::AddScore(int points)
{
    score += points;
    highScore = std::max(score, highScore);
    collectFlash = 0.15f;
}

void Game::Update()
{
    if (paused || gameOver) return;

    timeLeft = std::max(0.0f, timeLeft - deltaTime);
    if (timeLeft == 0.0f)
    {
        gameOver = true;
        return;
    }

    collectFlash = std::max(0.0f, collectFlash - deltaTime);
    player.Update(SDL_GetKeyboardState(nullptr), deltaTime, walls, worldWidth, worldHeight);

    float oldY = enemyRect.y;
    enemyRect.y += enemyVelocityY * deltaTime;
    if (enemyRect.y < 100.0f || enemyRect.y + enemyRect.h > worldHeight)
    {
        enemyRect.y = std::clamp(enemyRect.y, 100.0f, worldHeight - enemyRect.h);
        enemyVelocityY *= -1.0f;
    }
    for (const SDL_FRect& wall : walls)
    {
        if (IsColliding(enemyRect, wall))
        {
            enemyRect.y = oldY;
            enemyVelocityY *= -1.0f;
            break;
        }
    }

    if (IsColliding(player.GetRect(), enemyRect))
    {
        SDL_FRect nextTarget = enemyRect;
        if (FindSpawn(nextTarget))
        {
            enemyRect = nextTarget;
            targetsCaught++;
            AddScore(10);
            float speed = std::min(420.0f, 200.0f + targetsCaught * 20.0f);
            enemyVelocityY = enemyVelocityY < 0.0f ? -speed : speed;
        }
    }

    for (SDL_FRect& coin : coins)
    {
        if (IsColliding(player.GetRect(), coin))
        {
            SDL_FRect nextCoin = coin;
            if (FindSpawn(nextCoin))
            {
                coin = nextCoin;
                AddScore(1);
            }
        }
    }
}

void Game::Render()
{
    SDL_SetRenderDrawColor(renderer, 15, 15, 25, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 24, 24, 36, 255);
    for (int x = 0; x < worldWidth; x += 80)
    {
        SDL_RenderLine(renderer, static_cast<float>(x), 0.0f, static_cast<float>(x), worldHeight);
    }
    for (int y = 0; y < worldHeight; y += 80)
    {
        SDL_RenderLine(renderer, 0.0f, static_cast<float>(y), worldWidth, static_cast<float>(y));
    }

    SDL_SetRenderDrawColor(renderer, 85, 90, 110, 255);
    for (const SDL_FRect& wall : walls) SDL_RenderFillRect(renderer, &wall);

    SDL_SetRenderDrawColor(renderer, 245, 200, 65, 255);
    for (const SDL_FRect& coin : coins) SDL_RenderFillRect(renderer, &coin);

    SDL_SetRenderDrawColor(renderer, 65, 135, 245, 255);
    SDL_RenderFillRect(renderer, &enemyRect);

    SDL_FRect playerRect = player.GetRect();
    if (collectFlash > 0.0f) SDL_SetRenderDrawColor(renderer, 80, 230, 130, 255);
    else SDL_SetRenderDrawColor(renderer, 235, 75, 85, 255);
    SDL_RenderFillRect(renderer, &playerRect);

    SDL_SetRenderScale(renderer, 2.0f, 2.0f);
    SDL_SetRenderDrawColor(renderer, 240, 240, 245, 255);
    std::string hud = "Score: " + std::to_string(score) + "    Best: " + std::to_string(highScore)
        + "    Time: " + std::to_string(static_cast<int>(std::ceil(timeLeft)));
    SDL_RenderDebugText(renderer, 12.0f, 10.0f, hud.c_str());
    SDL_RenderDebugText(renderer, 12.0f, 25.0f, "WASD / Arrows: move   Shift: sprint   P: pause   R: restart");
    SDL_RenderDebugText(renderer, 12.0f, 40.0f, "Blue: 10 points   Gold: 1 point   F11: fullscreen   Esc: quit");

    if (paused || gameOver)
    {
        SDL_FRect panel = { 220.0f, 260.0f, 360.0f, 75.0f };
        SDL_SetRenderDrawColor(renderer, 35, 35, 55, 255);
        SDL_RenderFillRect(renderer, &panel);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDebugText(renderer, 250.0f, 280.0f,
            gameOver ? "TIME UP! Press R to play again" : "PAUSED. Press P to continue");
        SDL_RenderDebugText(renderer, 250.0f, 300.0f, hud.c_str());
    }
    SDL_SetRenderScale(renderer, 1.0f, 1.0f);
    SDL_RenderPresent(renderer);
}
