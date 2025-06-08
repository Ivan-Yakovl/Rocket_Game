#include "AirDefense.h"
#include <cmath>

AirDefense::AirDefense(const sf::Vector2f& position) : basePosition(position) {
    fuseSlider.setSize(sf::Vector2f(20.f, 200.f));
    fuseSlider.setFillColor(sf::Color(100, 100, 255));
    fuseSlider.setPosition(basePosition.x - 30.f, basePosition.y - 200.f);

    explosion.setRadius(0.f);
    explosion.setFillColor(sf::Color(255, 200, 50, 150));
    explosion.setOrigin(explosion.getRadius(), explosion.getRadius());
}

void AirDefense::activate() {
    active = true;
    predictedPath.clear();
}

void AirDefense::update(const Rocket& rocket, float deltaTime) {
    if (!active) return;

    predictionTime += deltaTime;
    if (predictionTime >= 0.5f) {
        predictPath(rocket);
        predictionTime = 0.f;
    }
}

void AirDefense::predictPath(const Rocket& rocket) {
    predictedPath.clear();
    if (!rocket.isMoving()) return; // Исправлено с isLaunched на isMoving

    sf::Vector2f pos = rocket.getPosition();
    sf::Vector2f target = predictedPath.empty() ? pos : predictedPath.back();
    sf::Vector2f vel = (pos - target) / 0.5f;

    for (float t = 0; t < 3.f; t += 0.1f) {
        predictedPath.push_back(pos + vel * t + sf::Vector2f(0.f, 0.5f * 9.8f * t * t));
    }
}

bool AirDefense::fire(const Rocket& rocket) {
    if (!active) return false;

    explosion.setRadius(40.f);
    explosion.setPosition(basePosition.x, basePosition.y - fuseHeight);
    return std::hypot(
        explosion.getPosition().x - rocket.getPosition().x,
        explosion.getPosition().y - rocket.getPosition().y
    ) < 50.f;
}

void AirDefense::draw(sf::RenderWindow& window) const {
    window.draw(fuseSlider);

    if (!predictedPath.empty()) {
        sf::VertexArray line(sf::LineStrip, predictedPath.size());
        for (size_t i = 0; i < predictedPath.size(); ++i) {
            line[i].position = predictedPath[i];
            line[i].color = sf::Color(255, 0, 0, 100);
        }
        window.draw(line);
    }

    if (explosion.getRadius() > 0.f) {
        window.draw(explosion);
    }
}