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

    // Установка радиуса и origin сразу
    effect.setRadius(currentRadius);
    effect.setOrigin(currentRadius, currentRadius);
}

void Explosion::draw(sf::RenderWindow& window) const {
    if (isActive()) {
        // Создаем временную копию для изменения
        sf::CircleShape temp = effect;
        temp.setRadius(currentRadius);
        temp.setOrigin(currentRadius, currentRadius);
        window.draw(temp);
    }
}

void Explosion::update(float deltaTime) {
    if (currentRadius < maxRadius) {
        currentRadius += deltaTime * 200.f;
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
    if (!isActive()) return false;

    // Более точная проверка с учетом радиуса
    float dx = point.x - effect.getPosition().x;
    float dy = point.y - effect.getPosition().y;
    float distanceSquared = dx * dx + dy * dy;
    float radiusSquared = currentRadius * currentRadius;

    return distanceSquared <= radiusSquared;
}

sf::Vector2f Explosion::getPosition() const {
    return effect.getPosition();
}

float Explosion::getRadius() const {
    return currentRadius;
}