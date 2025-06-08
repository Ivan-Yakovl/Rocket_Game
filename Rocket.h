#pragma once
#include <SFML/Graphics.hpp>
#include <functional>

class Rocket {
public:
    Rocket();

    void setTrajectory(std::function<sf::Vector2f(float)> trajectoryFunc, float duration);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window) const;
    bool isMoving() const;
    sf::Vector2f getPosition() const;
    float getCurrentTime() const;
    float getRadius() const { return sprite.getRadius(); }


private:
    sf::CircleShape sprite;
    std::function<sf::Vector2f(float)> trajectory;
    float flightDuration = 0.f;
    float currentTime = 0.f;
};