#include <SDL3/SDL_main.h>
#include "Game.hpp"

#include <exception>
#include <iostream>

int main(int, char*[])
{
    try
    {
        Game game;
        game.Run();
    }
    catch (const std::exception& error)
    {
        std::cerr << "Engine could not start: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
