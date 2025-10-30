#include "game.hpp"

#include <cmath>
#include <random>
#include <stdexcept>
#include <algorithm>
#include <string>
#include <vector>

using namespace std::literals;

Game::Game()
    : window(sf::VideoMode(sf::Vector2u(static_cast<unsigned>(WINDOW_WIDTH), static_cast<unsigned>(WINDOW_HEIGHT))), "Top-Down Survival")
    , enemySpawnTimer(0.f)
    , isGameOver(false)
{
    window.setFramerateLimit(60);

    // Load a font (adjust path if necessary)
    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        // Try relative path fallback (project can include a fonts/ folder)
        if (!font.openFromFile("./assets/fonts/arial.ttf")) {
            throw std::runtime_error("Could not load font 'arial.ttf'. Please provide a valid font at C:/Windows/Fonts/arial.ttf or ./assets/fonts/arial.ttf");
        }
    }

    player = createPlayer();
    gameClock.restart();
}

void Game::run()
{
    sf::Clock clock;

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        // SFML3: pollEvent returns std::optional<sf::Event>
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            // check for close
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            // allow Escape to quit
            if (event->is<sf::Event::KeyPressed>()) {
                const auto* k = event->getIf<sf::Event::KeyPressed>();
                if (k && k->code == sf::Keyboard::Key::Escape) window.close();
            }
        }

        if (!isGameOver) {
            handleInput(deltaTime);
            updateMovement(deltaTime);
            updateEnemyAI(deltaTime);
            spawnEnemies(deltaTime);
            checkCollisions();
        }

        window.clear(sf::Color::Black);
        render();
        updateDebugOverlay(deltaTime);

        if (isGameOver) {
            gameOver();
        }

        window.display();
    }
}

Entity Game::createPlayer()
{
    Entity entity = entityManager.createEntity();

    Transform transform;
    transform.position = sf::Vector2f(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f);
    transform.rotation = 0.f;
    transform.size = sf::Vector2f(20.f, 20.f);

    Shape shape;
    shape.type = ShapeType::Circle;
    shape.color = sf::Color::Green;
    shape.radius = 20.f;

    Health health;
    health.hp = 100;

    Velocity velocity;
    velocity.velocity = sf::Vector2f(0.f, 0.f);

    componentManager.addComponent(entity, transform);
    componentManager.addComponent(entity, shape);
    componentManager.addComponent(entity, health);
    componentManager.addComponent(entity, velocity);
    componentManager.addComponent(entity, PlayerTag{});

    return entity;
}

Entity Game::createEnemy()
{
    Entity entity = entityManager.createEntity();

    // Random position on screen edges
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> side(0, 3);
    std::uniform_real_distribution<float> xPos(0.f, static_cast<float>(WINDOW_WIDTH));
    std::uniform_real_distribution<float> yPos(0.f, static_cast<float>(WINDOW_HEIGHT));

    Transform transform;
    int spawnSide = side(gen);
    switch (spawnSide) {
        case 0: transform.position = sf::Vector2f(0.f, yPos(gen)); break;              // Left
        case 1: transform.position = sf::Vector2f(static_cast<float>(WINDOW_WIDTH), yPos(gen)); break;   // Right
        case 2: transform.position = sf::Vector2f(xPos(gen), 0.f); break;              // Top
        default: transform.position = sf::Vector2f(xPos(gen), static_cast<float>(WINDOW_HEIGHT)); break;  // Bottom
    }

    transform.rotation = 0.f;
    transform.size = sf::Vector2f(15.f, 15.f);

    Shape shape;
    shape.type = ShapeType::Circle;
    shape.color = sf::Color::Red;
    shape.radius = 15.f;

    Velocity velocity;
    velocity.velocity = sf::Vector2f(0.f, 0.f);

    componentManager.addComponent(entity, transform);
    componentManager.addComponent(entity, shape);
    componentManager.addComponent(entity, velocity);
    componentManager.addComponent(entity, EnemyTag{});

    return entity;
}

void Game::handleInput(float /*deltaTime*/)
{
    auto* velocity = componentManager.getComponent<Velocity>(player);
    if (!velocity) return;

    velocity->velocity = sf::Vector2f(0.f, 0.f);

    using K = sf::Keyboard::Key;
    if (sf::Keyboard::isKeyPressed(K::W) || sf::Keyboard::isKeyPressed(K::Up))
        velocity->velocity.y = -static_cast<float>(PLAYER_SPEED);
    if (sf::Keyboard::isKeyPressed(K::S) || sf::Keyboard::isKeyPressed(K::Down))
        velocity->velocity.y = static_cast<float>(PLAYER_SPEED);
    if (sf::Keyboard::isKeyPressed(K::A) || sf::Keyboard::isKeyPressed(K::Left))
        velocity->velocity.x = -static_cast<float>(PLAYER_SPEED);
    if (sf::Keyboard::isKeyPressed(K::D) || sf::Keyboard::isKeyPressed(K::Right))
        velocity->velocity.x = static_cast<float>(PLAYER_SPEED);
}

void Game::updateMovement(float deltaTime)
{
    for (Entity entity : entityManager.getEntities()) {
        auto* transform = componentManager.getComponent<Transform>(entity);
        auto* velocity = componentManager.getComponent<Velocity>(entity);

        if (!transform || !velocity) continue;

        transform->position += velocity->velocity * deltaTime;

        // Keep player in bounds
        if (componentManager.getComponent<PlayerTag>(entity)) {
            transform->position.x = std::clamp(transform->position.x, 0.f, static_cast<float>(WINDOW_WIDTH));
            transform->position.y = std::clamp(transform->position.y, 0.f, static_cast<float>(WINDOW_HEIGHT));
        }
    }
}

void Game::updateEnemyAI(float /*deltaTime*/)
{
    auto* playerTransform = componentManager.getComponent<Transform>(player);
    if (!playerTransform) return;

    for (Entity entity : entityManager.getEntities()) {
        if (!componentManager.getComponent<EnemyTag>(entity)) continue;

        auto* transform = componentManager.getComponent<Transform>(entity);
        auto* velocity = componentManager.getComponent<Velocity>(entity);

        if (!transform || !velocity) continue;

        // Move toward player
        sf::Vector2f direction = playerTransform->position - transform->position;
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (length > 0.0001f) {
            direction /= length;
            velocity->velocity = direction * static_cast<float>(ENEMY_SPEED);
        } else {
            velocity->velocity = sf::Vector2f(0.f, 0.f);
        }
    }
}

void Game::spawnEnemies(float deltaTime)
{
    enemySpawnTimer += deltaTime;
    if (enemySpawnTimer >= ENEMY_SPAWN_INTERVAL) {
        createEnemy();
        enemySpawnTimer = 0.f;
    }
}

bool Game::checkCircleCollision(const sf::Vector2f& pos1, float radius1,
                              const sf::Vector2f& pos2, float radius2)
{
    float dx = pos1.x - pos2.x;
    float dy = pos1.y - pos2.y;
    float distanceSq = dx * dx + dy * dy;
    float r = radius1 + radius2;
    return distanceSq < (r * r);
}

void Game::checkCollisions()
{
    auto* playerTransform = componentManager.getComponent<Transform>(player);
    auto* playerShape = componentManager.getComponent<Shape>(player);
    auto* playerHealth = componentManager.getComponent<Health>(player);

    if (!playerTransform || !playerShape || !playerHealth) return;

    std::vector<Entity> entitiesToDestroy;

    for (Entity entity : entityManager.getEntities()) {
        if (!componentManager.getComponent<EnemyTag>(entity)) continue;

        auto* enemyTransform = componentManager.getComponent<Transform>(entity);
        auto* enemyShape = componentManager.getComponent<Shape>(entity);

        if (!enemyTransform || !enemyShape) continue;

        if (checkCircleCollision(playerTransform->position, playerShape->radius,
                               enemyTransform->position, enemyShape->radius)) {
            playerHealth->hp -= 10;
            entitiesToDestroy.push_back(entity);

            if (playerHealth->hp <= 0) {
                isGameOver = true;
            }
        }
    }

    for (Entity e : entitiesToDestroy) {
        entityManager.destroyEntity(e);
        componentManager.removeEntity(e);
    }
}

void Game::render()
{
    for (Entity entity : entityManager.getEntities()) {
        auto* transform = componentManager.getComponent<Transform>(entity);
        auto* shape = componentManager.getComponent<Shape>(entity);

        if (!transform || !shape) continue;

        // Draw circle shapes only (we use circle for both player and enemies)
        sf::CircleShape circle(shape->radius);
        circle.setFillColor(shape->color);
        circle.setOrigin(sf::Vector2f(shape->radius, shape->radius));
        circle.setPosition(transform->position);
        window.draw(circle);
    }
}

void Game::updateDebugOverlay(float /*deltaTime*/)
{
    int entityCount = static_cast<int>(entityManager.getEntities().size());
    int enemyCount = 0;
    for (Entity entity : entityManager.getEntities()) {
        if (componentManager.getComponent<EnemyTag>(entity)) ++enemyCount;
    }

    auto* playerHealth = componentManager.getComponent<Health>(player);
    int hp = playerHealth ? playerHealth->hp : 0;

    float elapsedTime = gameClock.getElapsedTime().asSeconds();

    std::string debugString =
        "Entities: " + std::to_string(entityCount) +
        "\nEnemies: " + std::to_string(enemyCount) +
        "\nPlayer HP: " + std::to_string(hp) +
        "\nTime: " + std::to_string(static_cast<int>(elapsedTime));

    // SFML3: construct Text with a Font
    sf::Text debugText(font, debugString, 14);
    debugText.setFillColor(sf::Color::White);
    debugText.setPosition(sf::Vector2f(10.f, 10.f));
    window.draw(debugText);
}

void Game::gameOver()
{
    sf::Text gameOverText(font, "GAME OVER", 50);
    gameOverText.setFillColor(sf::Color::Red);

    sf::FloatRect bounds = gameOverText.getLocalBounds();
    // FloatRect uses position and size in SFML3
    sf::Vector2f center(bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f);
    gameOverText.setOrigin(center);
    gameOverText.setPosition(sf::Vector2f(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f));

    window.draw(gameOverText);
}