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
ctest --test-dir build
./build/GameEngine
```

Pass `-DGAMEENGINE_BUILD_TESTS=OFF` to skip the registry tests.

The code needs SDL 3.2 or newer. It uses SDL's basic built in debug font, so no font files or SDL_ttf are required.

## Architecture

The engine uses a small entity component system. Entities are ids, components are plain structs and systems are functions that run over every entity with the components they need.

```text
src/
  engine/       reusable engine code, no game rules
  game/         this game, built on the engine
  main.cpp      starts the game
tests/          headless registry tests
```

### Engine

| File | Contents |
| --- | --- |
| Entity.hpp | Entity id with an index and a generation, so stale handles are rejected after an index is reused. |
| ComponentPool.hpp | Sparse set storage. Components sit packed in one array for fast iteration and are removed by swapping with the last slot. |
| Registry.hpp | Creates and destroys entities, adds, gets and removes components, and runs `View<Ts...>` over entities that have every listed component. |
| Components.hpp | Transform, Velocity, Collider, Solid, Bounds and Renderable. |
| Systems.cpp | MovementSystem moves one axis at a time and slides or bounces against Solid entities and Bounds. FlashSystem and RenderSystem handle drawing. |
| Collision.hpp | The rectangle overlap check. |

### Game

| File | Contents |
| --- | --- |
| GameComponents.hpp | PlayerControl, Pickup, Target and Wall. |
| GameState.hpp | Score, best score, timer, pause and game over. |
| GameSystems.cpp | InputSystem, PickupSystem, HudSystem and spawn placement. |
| Game.cpp | SDL setup, the main loop, input events and creating the entities for each round. |

Each frame runs Flash, Input, Movement and Pickup systems, then Render and HUD.
Restarting clears the registry and creates the walls, player, target and pickups again.

A new object is usually just a new entity. For example, a wall is Transform, Collider, Solid, Wall and Renderable.

## Validation

The ECS version built against SDL 3.2 with CMake and GCC with Wall and Wextra and no warnings. The registry tests pass, and a headless harness checked normalized diagonal speed, sliding along a wall, target bouncing, catching the target and the green flash.


The complete project built against SDL 3.2.10 using CMake and GCC with Wall, Wextra and Werror enabled.
A C++ harness using real SDL checked movement, collisions on all four sides, sprinting, 1,000 spawn layouts, scoring, pause, restart, timer expiry, target bouncing and quit handling. Rendering was exercised with the SDL dummy video driver.
Desktop gameplay and the Windows Visual Studio build still need to be tested on your machine.
