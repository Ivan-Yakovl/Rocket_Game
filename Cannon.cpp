#include "Cannon.h"

Cannon::Cannon(const sf::Vector2f& position) {
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
}

void Cannon::update(float deltaTime) {
    fireCooldown -= deltaTime;

    float angleRad = rotation * 3.14159f / 180.f;
    float moveX = std::cos(angleRad) * projectileSpeed * deltaTime;
    float moveY = std::sin(angleRad) * projectileSpeed * deltaTime;

    for (auto& projectile : projectiles) {
        projectile.move(moveX, moveY);
    }

    // Удаление снарядов за границами экрана
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
            [](const sf::CircleShape& p) {
        auto pos = p.getPosition();
        return pos.x < -50 || pos.x > 1074 || pos.y < -50 || pos.y > 818;
    }),
        projectiles.end()
    );
}

void Cannon::draw(sf::RenderTarget& target) const {
    target.draw(base);
    target.draw(barrel);

    for (const auto& projectile : projectiles) {
        target.draw(projectile);
    }
}

void Cannon::fire() {
    if (fireCooldown <= 0.f) {
        sf::CircleShape projectile(8.f);
        projectile.setFillColor(sf::Color(255, 255, 0));
        projectile.setOrigin(8.f, 8.f);
        projectile.setPosition(getBarrelPosition());
        projectiles.push_back(projectile);
        fireCooldown = fireRate;
    }
}

void Cannon::rotate(float angleDelta) {
    rotation += angleDelta;
    barrel.setRotation(rotation);
}

std::vector<sf::CircleShape>& Cannon::getProjectiles() {
    return projectiles;
}

const std::vector<sf::CircleShape>& Cannon::getProjectiles() const {
    return projectiles;
}

float Cannon::getRotation() const {
    return rotation;
}

sf::Vector2f Cannon::getBarrelPosition() const {
    return barrel.getTransform().transformPoint({ 50, 5 });
}