# How the Engine Works

A short tour of what happens under the hood. The engine is a small entity component system (ECS) on top of SDL3.

## The big idea

Instead of a `Player` class that owns its position, movement and drawing, everything in the game is an **entity**: just an id. Data is attached as **components** (plain structs), and **systems** are functions that loop over every entity with a certain set of components. A wall, the player and a pickup are all the same kind of thing, with different components attached.

## Entities and generations

An `Entity` is two numbers: an `index` and a `generation`. The index is a slot in the registry's arrays. When an entity is destroyed, its slot goes on a free list to be reused, and its generation goes up by one.

That way an old handle to a destroyed entity can't accidentally point at the new entity using the same slot. The indices match, but the generations don't, so `IsAlive` returns false.

## Component pools (sparse sets)

Each component type has its own `ComponentPool<T>`, which is a sparse set made of three arrays:

- `sparse`: entity index → position in the dense arrays (or "empty")
- `dense`: the entity index stored at each position
- `components`: the actual data, packed tightly

Lookups are O(1) through `sparse`. Iterating goes straight down the packed `components` array, which is cache friendly. Removing swaps the item with the last one and pops it off, so the arrays never have gaps.

## The Registry and views

The `Registry` owns the entities and all the pools. It gives each component type a small number the first time it's used and creates that type's pool on demand. The main calls are `Create`, `Destroy`, `Add<T>`, `Get<T>`, `Remove<T>`, `Has<Ts...>` and `View<Ts...>`.

`View<Transform, Velocity>(fn)` calls `fn` for every entity that has both. It walks the **smallest** of those pools and checks whether each entity also has the other components, so a view over a rare component stays cheap. It works on a copy of the entity list, so destroying an entity during a view is safe.

## Components

- **Engine:** `Transform` (position), `Velocity`, `Collider` (size, plus how it reacts to walls: slide, bounce or none), `Solid` (blocks movement), `Bounds` (an area to stay inside) and `Renderable` (colour, draw layer, flash colour).
- **Game:** `PlayerControl`, `Pickup`, `Target`, `Wall`. Round data like score and time lives in a plain `GameState`, not in entities.

## Systems and frame order

Each frame the loop caps delta time at 50 ms, handles SDL events (quit, pause, restart, fullscreen, focus loss), and then, unless paused or game over, runs:

1. **Flash:** counts down the green collect flash.
2. **Input:** reads the keyboard and sets the player's velocity. Diagonal input is normalized and Shift multiplies speed.
3. **Movement:** moves everything that has a velocity and resolves collisions.
4. **Pickup:** checks whether the player touches the target or a pickup, adds score and respawns it somewhere free.

After that, **Render** draws all renderables sorted by layer and **HUD** draws the text. The order matters: input sets velocity before movement uses it, and pickups are checked against the new positions.

## Collision and sliding

Movement handles one axis at a time. Move on X, then check for overlaps with solids. If the entity hit one, snap it flush against the wall's edge. Then do the same on Y. Because the axes are separate, running diagonally into a wall stops you on one axis while you keep sliding along the other. Bouncing entities (the blue target) step back to their previous position and flip their velocity instead. `Bounds` keeps movers inside the play area.

## Restart

Pressing R resets `GameState` (keeping the best score), calls `registry.Clear()` and rebuilds the round. Walls go first, then the player and target, then six pickups placed by `FindSpawn`. `FindSpawn` tries random spots that don't overlap anything and falls back to a grid scan if the map is crowded.
