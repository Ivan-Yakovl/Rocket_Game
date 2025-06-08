#pragma once
#include <SFML/Graphics.hpp>
#include "Rocket.h"

class AirDefense {
public:
    AirDefense(const sf::Vector2f& position);
    void activate();
    void update(const Rocket& rocket, float deltaTime);
    void predictPath(const Rocket& rocket);
    bool fire(const Rocket& rocket);
    void draw(sf::RenderWindow& window) const;

private:
    sf::Vector2f basePosition;
    sf::RectangleShape fuseSlider;
    sf::CircleShape explosion;
    std::vector<sf::Vector2f> predictedPath;
    float predictionTime = 0.f;
    float fuseHeight = 100.f;
    bool active = false;
};