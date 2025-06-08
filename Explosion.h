#pragma once
#include <SFML/Graphics.hpp>

class Explosion {
public:
    Explosion();

    void trigger(const sf::Vector2f& position);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window) const;
    void reset();
    bool isActive() const;
    bool intersects(const sf::Vector2f& point) const;
    sf::Vector2f getPosition() const;
    float getRadius() const;

private:
    mutable sf::CircleShape effect; // Добавлен mutable
    float maxRadius;
    float currentRadius;
    float duration;
};