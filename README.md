# Top-Down Survival Game

A simple top-down survival game built with C++17 and SFML, featuring a minimal Entity-Component-System (ECS) architecture.

## Features

- Player movement with WASD or arrow keys
- Enemy spawning from screen edges
- Basic collision detection
- Health system
- Debug overlay showing game stats
- Simple ECS implementation

## Controls

- WASD or Arrow Keys: Move the player
- Escape: Close the game
- Debug overlay shows entity count, enemy count, player HP, and elapsed time

## Requirements

- C++17 compatible compiler
- CMake 3.14 or higher
- SFML 2.5 or higher

## Build Instructions

1. Create a build directory:
```bash
mkdir build
cd build
```

2. Configure and build the project:
```bash
cmake ..
cmake --build .
```

3. Run the game:
```bash
./TopDownSurvival  # On Unix-like systems
TopDownSurvival.exe  # On Windows
```

## Game Rules

- Control the green circle (player) to avoid red circles (enemies)
- Enemies spawn periodically from the screen edges
- Colliding with enemies reduces player HP by 10
- Game ends when player HP reaches 0

## Implementation Details

### ECS Architecture

The game uses a simple ECS architecture with the following components:

- Transform (position, rotation, size)
- Velocity (movement vector)
- Shape (visual representation)
- Health (HP system)
- PlayerTag and EnemyTag (entity identification)

### Systems

- Input System: Handles player movement
- Movement System: Updates entity positions
- Enemy AI System: Controls enemy behavior
- Spawning System: Creates new enemies
- Collision System: Handles entity collisions
- Rendering System: Draws entities
- Debug System: Shows game statistics

## Possible Extensions

1. Add projectiles and shooting mechanics
2. Implement power-ups
3. Add different enemy types
4. Include sound effects and music
5. Add scoring system
6. Implement game states (menu, pause, etc.)
7. Add visual effects (particles, screen shake)
8. Include different player abilities