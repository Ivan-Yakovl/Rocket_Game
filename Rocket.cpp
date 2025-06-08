#include "Rocket.h"
#include <cmath>

Rocket::Rocket() {
    sprite.setRadius(10.f);
    sprite.setFillColor(sf::Color::Red);
    sprite.setOrigin(10.f, 10.f);
}

void Rocket::setTrajectory(std::function<sf::Vector2f(float)> trajectoryFunc, float duration) {
    trajectory = trajectoryFunc;
    flightDuration = duration;
    currentTime = 0.f;
}

void Rocket::update(float deltaTime) {
    if (currentTime <= flightDuration) {
        currentTime += deltaTime;
        float t = currentTime / flightDuration;
        sprite.setPosition(trajectory(t));
    }
}

void Rocket::draw(sf::RenderWindow& window) const {
    if (isMoving()) {
        window.draw(sprite);
    }
}

bool Rocket::isMoving() const {
    return currentTime <= flightDuration;
}

sf::Vector2f Rocket::getPosition() const {
    return sprite.getPosition();
}

float Rocket::getCurrentTime() const {
    return currentTime;
}