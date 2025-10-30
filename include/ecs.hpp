#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <vector>
#include <any>
#include <variant>

// Entity is just an ID
using Entity = std::size_t;

// Component types
struct Transform {
    sf::Vector2f position;
    float rotation;
    sf::Vector2f size;
};

struct Velocity {
    sf::Vector2f velocity;
};

enum class ShapeType {
    Circle,
    Rectangle
};

struct Shape {
    ShapeType type;
    sf::Color color;
    float radius;  // Used for circles and collision
};

struct Health {
    int hp;
};

struct PlayerTag {};
struct EnemyTag {};

// Component Manager
class ComponentManager {
private:
    // Component storage
    std::unordered_map<std::type_index, std::unordered_map<Entity, std::any>> components;

public:
    template<typename T>
    void addComponent(Entity entity, T component) {
        std::type_index typeId(typeid(T));
        components[typeId][entity] = component;
    }

    template<typename T>
    void removeComponent(Entity entity) {
        std::type_index typeId(typeid(T));
        if (components.count(typeId)) {
            components[typeId].erase(entity);
        }
    }

    template<typename T>
    T* getComponent(Entity entity) {
        std::type_index typeId(typeid(T));
        auto& componentMap = components[typeId];
        auto it = componentMap.find(entity);
        if (it != componentMap.end()) {
            return std::any_cast<T>(&it->second);
        }
        return nullptr;
    }

    void removeEntity(Entity entity) {
        for (auto& [_, componentMap] : components) {
            componentMap.erase(entity);
        }
    }
};

// Entity Manager
class EntityManager {
private:
    std::vector<Entity> entities;
    Entity nextEntity = 0;

public:
    Entity createEntity() {
        Entity entity = nextEntity++;
        entities.push_back(entity);
        return entity;
    }

    void destroyEntity(Entity entity) {
        entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
    }

    const std::vector<Entity>& getEntities() const {
        return entities;
    }
};