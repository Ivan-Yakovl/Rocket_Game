#pragma once
#include <SFML/Graphics.hpp>

class RocketLauncher {
public:
    RocketLauncher(const sf::Vector2f& position);
    void update(float deltaTime);
    void draw(sf::RenderTarget& target) const;
    void fire();
    void explode();
    void rotateRocket(float angleDelta);
    void accelerateRocket(float speedDelta);
    void reset();
    sf::Vector2f getBarrelPosition() const;

    bool isRocketActive() const;
    sf::Vector2f getRocketPosition() const;
    float getRocketRadius() const;
    bool checkRocketHit(const sf::Vector2f& targetPos) const;

private:
    sf::ConvexShape base;
    sf::RectangleShape barrel;

    struct GuidedRocket {
        sf::CircleShape shape;
        sf::Vector2f velocity;
        bool active = false;
        float speed = 300.f;
        float angle = 90.f;
    } rocket;
};