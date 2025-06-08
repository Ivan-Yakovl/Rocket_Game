#include "Explosion.h"

Explosion::Explosion() : maxRadius(0.f), currentRadius(0.f), duration(0.f) {
    effect.setFillColor(sf::Color(255, 150, 50, 180));
    effect.setOrigin(0.f, 0.f);
}

void Explosion::trigger(const sf::Vector2f& position) {
    effect.setPosition(position);
    maxRadius = 60.f;
    currentRadius = 5.f;
    duration = 0.3f;
}

void Explosion::draw(sf::RenderWindow& window) const {
    if (isActive()) {
        window.draw(effect);
    }
}

void Explosion::update(float deltaTime) {
    if (currentRadius < maxRadius) {
        currentRadius += deltaTime * 200.f;
        effect.setRadius(currentRadius);
        effect.setOrigin(currentRadius, currentRadius);
    }
    else if (isActive()) {
        currentRadius = 0.f;
    }
}

void Explosion::reset() {
    currentRadius = 0.f;
    maxRadius = 0.f;
    duration = 0.f;
}

bool Explosion::isActive() const {
    return currentRadius > 0 && currentRadius < maxRadius;
}

bool Explosion::intersects(const sf::Vector2f& point) const {
    return isActive() &&
        std::hypot(point.x - effect.getPosition().x, point.y - effect.getPosition().y) <= currentRadius;
}