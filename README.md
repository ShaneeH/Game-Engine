# Shane Engine

A small C++ game engine project using SDL3.

Catch the blue target and collect gold squares before the 60 second round ends.
Blue targets give 10 points and speed up after each catch. Gold squares give 1 point.
The best score stays available when restarting, until the program closes.

## Added in this version

1. Solid walls with collision detection and sliding along their edges.
2. Sprinting and arrow key support. Diagonal movement has the same speed as straight movement.
3. Six bonus pickups that respawn away from walls, the player and other pickups.
4. A round timer, game over screen, pause and restart.
5. Score, best score, timer and controls drawn inside the window.
6. A short green flash when collecting something.
7. Resizing with a fixed game area, fullscreen toggle and automatic pause on focus loss.
8. SDL startup error handling and a CMake build option.

The player and target are smaller to leave room to move around the walls.
The blue rectangle still acts as the moving target from the original game.
Movement now uses the current frame time, capped at 50 milliseconds to avoid large jumps after a stall.
The timer uses that same capped game time, so a heavily stalled game runs slower than real time.

## Controls

| Key | Action |
| --- | --- |
| WASD or arrow keys | Move |
| Shift | Sprint |
| P | Pause or resume |
| R | Restart the round |
| F11 | Toggle fullscreen |
| Escape | Quit |

## Visual Studio

Open GameEngine.vcxproj and choose Debug or Release with x64.
The project keeps your v145 toolset and defaults SDLRoot to C:\Users\shane\SDL3-3.4.8.
Change SDLRoot in the project file if SDL is installed somewhere else.
If your Visual Studio version does not have v145, retarget the project to your installed C++ toolset.
SDL3.dll is copied beside the executable when it is present in the SDL lib directory.
Otherwise, copy the matching SDL3.dll beside the executable yourself.
Win32 needs the x86 SDL libraries.

## Fedora

Install SDL3 development files and CMake, then build from this folder:

```sh
sudo dnf install gcc-c++ cmake SDL3-devel
cmake -S . -B build
cmake --build build
./build/GameEngine
```

The code needs SDL 3.2 or newer. It uses SDL's basic built in debug font, so no font files or SDL_ttf are required.

## Files

Player.cpp handles movement and wall collisions.
Collision.hpp contains the rectangle overlap check.
src/Game.cpp handles the loop, round state, target, pickups and rendering.
src/main.cpp starts the game.

## Validation

The complete project built against SDL 3.2.10 using CMake and GCC with Wall, Wextra and Werror enabled.
A C++ harness using real SDL checked movement, collisions on all four sides, sprinting, 1,000 spawn layouts, scoring, pause, restart, timer expiry, target bouncing and quit handling. Rendering was exercised with the SDL dummy video driver.
Desktop gameplay and the Windows Visual Studio build still need to be tested on your machine.
