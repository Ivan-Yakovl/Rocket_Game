#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class PredictionSystem {
public:
    void update(const std::vector<sf::Vector2f>& recentPositions, float deltaTime);
    void draw(sf::RenderWindow& window) const;
    void reset();

private:
    sf::Vector2f calculateTaylorPrediction(const std::vector<sf::Vector2f>& samples, float time) const;
    sf::Vector2f calculateVelocity(const std::vector<sf::Vector2f>& samples) const;
    sf::Vector2f calculateAcceleration(const std::vector<sf::Vector2f>& samples) const;
    sf::Vector2f calculateJerk(const sf::Vector2f& a1, const sf::Vector2f& a2) const;

    std::vector<sf::Vector2f> predictedPath;
};