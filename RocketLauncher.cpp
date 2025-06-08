#include "RocketLauncher.h"

RocketLauncher::RocketLauncher(const sf::Vector2f& position) {
    base.setPointCount(4);
    base.setPoint(0, { 10, 40 });
    base.setPoint(1, { 30, 40 });
    base.setPoint(2, { 40, 0 });
    base.setPoint(3, { 0, 0 });
    base.setPosition(position);
    base.setFillColor(sf::Color(150, 150, 150));

    barrel.setSize({ 50, 10 });
    barrel.setOrigin({ 0, 5 });
    barrel.setPosition(position.x + 20, position.y + 20);
    barrel.setFillColor(sf::Color(100, 100, 100));
    barrel.setRotation(270.f);

    rocket.shape.setRadius(8.f);
    rocket.shape.setFillColor(sf::Color(0, 255, 255));
    rocket.shape.setOrigin(8.f, 8.f);
}

float RocketLauncher::getRocketRadius() const {
    return rocket.shape.getRadius();
}

bool RocketLauncher::checkRocketHit(const sf::Vector2f& targetPos) const {
    if (!rocket.active) return false;

    sf::Vector2f rocketPos = rocket.shape.getPosition();
    float dx = rocketPos.x - targetPos.x;
    float dy = rocketPos.y - targetPos.y;
    float distanceSquared = dx * dx + dy * dy;
    float radius = rocket.shape.getRadius();

    return distanceSquared <= (radius * radius);
}

void RocketLauncher::update(float deltaTime) {
    if (rocket.active) {
        float angleRad = rocket.angle * 3.14159f / 180.f;
        rocket.velocity = sf::Vector2f(std::cos(angleRad), std::sin(angleRad)) * rocket.speed;
        rocket.shape.move(rocket.velocity * deltaTime);

        auto pos = rocket.shape.getPosition();
        if (pos.x < -50 || pos.x > 1074 || pos.y < -50 || pos.y > 818) {
            rocket.active = false;
        }
    }
}

void RocketLauncher::draw(sf::RenderTarget& target) const {
    target.draw(base);
    target.draw(barrel);
    if (rocket.active) {
        target.draw(rocket.shape);
    }
}

void RocketLauncher::fire() {
    if (!rocket.active) {
        rocket.active = true;
        rocket.angle = 270.f;
        rocket.speed = 300.f;
        rocket.shape.setPosition(barrel.getPosition() + sf::Vector2f(0, -50));
    }
}

void RocketLauncher::explode() {
    rocket.active = false;
}

void RocketLauncher::rotateRocket(float angleDelta) {
    if (rocket.active) {
        rocket.angle += angleDelta;
        if (rocket.angle < 0) rocket.angle += 360;
        if (rocket.angle >= 360) rocket.angle -= 360;
    }
}

void RocketLauncher::accelerateRocket(float speedDelta) {
    if (rocket.active) {
        rocket.speed += speedDelta;
        rocket.speed = std::max(100.f, std::min(800.f, rocket.speed));
    }
}

void RocketLauncher::reset() {
    rocket.active = false;
}

bool RocketLauncher::isRocketActive() const {
    return rocket.active;
}

sf::Vector2f RocketLauncher::getRocketPosition() const {
    return rocket.shape.getPosition();
}
sf::Vector2f RocketLauncher::getBarrelPosition() const {
    return barrel.getTransform().transformPoint(sf::Vector2f(50, 5));
}