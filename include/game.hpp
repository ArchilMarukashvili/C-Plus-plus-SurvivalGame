#pragma once
#include "ecs.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

class Game {
public:
    Game();
    void run();

private:
    // Window settings
    static constexpr int WINDOW_WIDTH = 800;
    static constexpr int WINDOW_HEIGHT = 600;
    static constexpr int PLAYER_SPEED = 200;
    static constexpr int ENEMY_SPEED = 100;
    static constexpr float ENEMY_SPAWN_INTERVAL = 2.0f;

    // Game systems
    void handleInput(float deltaTime);
    void updateMovement(float deltaTime);
    void updateEnemyAI(float deltaTime);
    void spawnEnemies(float deltaTime);
    void checkCollisions();
    void render();
    void updateDebugOverlay(float deltaTime);
    void gameOver();

    // Helper functions
    Entity createPlayer();
    Entity createEnemy();
    bool checkCircleCollision(const sf::Vector2f& pos1, float radius1,
                            const sf::Vector2f& pos2, float radius2);

    // SFML
    sf::RenderWindow window;
    sf::Font font;
    sf::Clock gameClock;
    float enemySpawnTimer;

    // ECS
    EntityManager entityManager;
    ComponentManager componentManager;
    Entity player;

    // Game state
    bool isGameOver;
};