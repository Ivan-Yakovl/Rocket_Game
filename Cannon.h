#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Cannon {
public:
    Cannon(const sf::Vector2f& position);
    void update(float deltaTime);
    void draw(sf::RenderTarget& target) const;
    void fire();
    void rotate(float angleDelta);
    std::vector<sf::CircleShape>& getProjectiles();
    const std::vector<sf::CircleShape>& getProjectiles() const;
    float getRotation() const;
    sf::Vector2f getBarrelPosition() const;

private:
    sf::ConvexShape base;
    sf::RectangleShape barrel;
    std::vector<sf::CircleShape> projectiles;
    float rotation = 5.f;
    float projectileSpeed = 800.f;
    float fireRate = 0.5f;
    float fireCooldown = 1.f;
};