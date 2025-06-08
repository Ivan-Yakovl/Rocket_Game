#include "PredictionSystem.h"
#include <SFML/Graphics.hpp>

#pragma warning(push)
#pragma warning(disable: 26495)
#include <SFML/Graphics.hpp>
#pragma warning(pop)
void PredictionSystem::update(const std::vector<sf::Vector2f>& recentPositions, float deltaTime) {
    if (recentPositions.size() < 4) return;

    predictedPath.clear();

    // Увеличено количество точек предсказания (с 60 до 100 точек)
    for (float t = 0; t < 2.0f; t += 0.02f) {  // Увеличен временной диапазон с 3 до 5 секунд
        predictedPath.push_back(calculateTaylorPrediction(recentPositions, t));
    }
}

sf::Vector2f PredictionSystem::calculateTaylorPrediction(const std::vector<sf::Vector2f>& samples, float time) const {
    sf::Vector2f velocity = calculateVelocity(samples);
    sf::Vector2f acceleration = calculateAcceleration(samples);
    sf::Vector2f jerk = calculateJerk(
        samples[samples.size() - 1] - samples[samples.size() - 2],
        samples[samples.size() - 2] - samples[samples.size() - 3]
    );

    return samples.back() +
        velocity * time +
        acceleration * (time * time) * 0.5f +
        jerk * (time * time * time) / 6.f;
}

sf::Vector2f PredictionSystem::calculateVelocity(const std::vector<sf::Vector2f>& samples) const {
    // Используем усреднение для плавности
    if (samples.size() < 2) return sf::Vector2f(0, 0);
    return (samples.back() - samples[samples.size() - 2]) / 0.1f;
}

sf::Vector2f PredictionSystem::calculateAcceleration(const std::vector<sf::Vector2f>& samples) const {
    if (samples.size() < 3) return sf::Vector2f(0, 0);
    
    // Усредняем ускорение по нескольким точкам
    sf::Vector2f totalAcceleration(0, 0);
    int count = 0;
    
    for (size_t i = 2; i < samples.size(); ++i) {
        sf::Vector2f v1 = (samples[i] - samples[i-1]) / 0.1f;
        sf::Vector2f v2 = (samples[i-1] - samples[i-2]) / 0.1f;
        totalAcceleration += (v1 - v2) / 0.1f;
        count++;
    }
    
    return count > 0 ? totalAcceleration / static_cast<float>(count) : sf::Vector2f(0, 0);
}

sf::Vector2f PredictionSystem::calculateJerk(const sf::Vector2f& a1, const sf::Vector2f& a2) const {
    return (a1 - a2) / 0.1f;
}

void PredictionSystem::draw(sf::RenderWindow& window) const {
    if (predictedPath.size() < 2) return;

    // Рисуем плавную линию с большим количеством точек
    sf::VertexArray line(sf::LineStrip, predictedPath.size());
    for (size_t i = 0; i < predictedPath.size(); ++i) {
        line[i].position = predictedPath[i];

        // Плавное изменение прозрачности
        float alpha = 200 * (1.0f - static_cast<float>(i) / predictedPath.size());
        line[i].color = sf::Color(255, 50, 50, static_cast<sf::Uint8>(alpha));
    }
    window.draw(line);

    // Дополнительно рисуем точки для лучшей визуализации
    for (size_t i = 0; i < predictedPath.size(); i += 5) {
        sf::CircleShape point(3.f);
        point.setFillColor(sf::Color(255, 100, 100, 150));
        point.setOrigin(3.f, 3.f);
        point.setPosition(predictedPath[i]);
        window.draw(point);
    }
}

void PredictionSystem::reset() {
    predictedPath.clear();
}