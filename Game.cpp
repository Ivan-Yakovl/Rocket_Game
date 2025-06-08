#include "Game.h"
#include <algorithm>
#include <iostream>

#pragma warning(push)
#pragma warning(disable: 26495)
#include <SFML/Graphics.hpp>
#pragma warning(pop)

Game::Game() :
    window(sf::VideoMode(1024, 768), "Rocket Defense"),
    defenseCannon(sf::Vector2f(100.f, 700.f)),
    currentState(State::SelectingTrajectory),
    selectedTrajectory(0),
    score(0),
    rocketHealth(5)
{
    window.setVerticalSyncEnabled(true);

    if (!font.loadFromFile("arial.ttf")) {
        font.loadFromFile("C:/Windows/Fonts/arial.ttf");
    }

    initTrajectories();
}

void Game::run() {
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        processEvents();
        update(deltaTime);
        render();
    }
}

void Game::initTrajectories() {
    availableTrajectories.clear();

    // Синусоида
    availableTrajectories.push_back({
        [](float t) -> sf::Vector2f {
            return sf::Vector2f(
                100.f + t * 900.f,
                300.f + 80.f * std::sin(t * 3.14f * 4.f)
            );
        },
        "Sinusoid"
        });

    // Парабола
    availableTrajectories.push_back({
        [](float t) -> sf::Vector2f {
            return sf::Vector2f(
                100.f + t * 900.f,
                500.f - 300.f * t * (1.f - t)
            );
        },
        "Parabola"
        });

    // Прямая
    availableTrajectories.push_back({
        [](float t) -> sf::Vector2f {
            return sf::Vector2f(
                100.f + t * 900.f,
                200.f + t * 300.f
            );
        },
        "Straight Line"
        });

    // Квадратичная
    availableTrajectories.push_back({
        [](float t) -> sf::Vector2f {
            float offset = t - 0.5f;
            return sf::Vector2f(
                100.f + t * 900.f,
                400.f - 200.f * (offset * offset) * 4.f
            );
        },
        "Quadratic"
        });

    // Спираль
    availableTrajectories.push_back({
        [](float t) -> sf::Vector2f {
            return sf::Vector2f(
                100.f + t * 900.f,
                300.f + 150.f * std::sin(t * 3.14f * 6.f) * (1 - t)
            );
        },
        "Spiral"
        });

    // Логарифмическая
    availableTrajectories.push_back({
        [](float t) -> sf::Vector2f {
            return sf::Vector2f(
                100.f + t * 900.f,
                200.f + 100.f * std::log1p(t * 5.f)
            );
        },
        "Logarithmic"
        });
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        if (currentState == State::SelectingTrajectory) {
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Left) {
                    selectedTrajectory = (selectedTrajectory - 1 + static_cast<int>(availableTrajectories.size())) % static_cast<int>(availableTrajectories.size());
                }
                else if (event.key.code == sf::Keyboard::Right) {
                    selectedTrajectory = (selectedTrajectory + 1) % static_cast<int>(availableTrajectories.size());
                }
                else if (event.key.code == sf::Keyboard::Enter && !availableTrajectories.empty()) {
                    // Увеличена скорость ракеты - уменьшено время полета с 10 до 7 секунд
                    playerRocket.setTrajectory(availableTrajectories[selectedTrajectory].function, 7.f);
                    rocketHealth = 5;
                    currentState = State::DefensePhase;
                    rocketPositionsHistory.clear();
                }
            }
        }
        else if (currentState == State::DefensePhase && event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Space) {
                defenseCannon.fire();
            }
        }
        else if (currentState == State::GameOver && event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Enter) {
                restartGame();
            }
        }
    }
}

void Game::restartGame() {
    currentState = State::SelectingTrajectory;
    selectedTrajectory = 0;
    rocketHealth = 5;
    rocketPositionsHistory.clear();
    predictionSystem.reset();
    explosion.reset();
}

void Game::update(float deltaTime) {
    if (currentState == State::DefensePhase) {
        // Уменьшена скорость поворота с 90 до 60 градусов/сек
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            defenseCannon.rotate(-60.f * deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            defenseCannon.rotate(60.f * deltaTime);
        }

        defenseCannon.update(deltaTime);
        playerRocket.update(deltaTime);

        if (playerRocket.isMoving()) {
            rocketPositionsHistory.push_back(playerRocket.getPosition());
            if (rocketPositionsHistory.size() > MAX_HISTORY) {
                rocketPositionsHistory.erase(rocketPositionsHistory.begin());
            }
        }

        std::vector<sf::CircleShape>& projectiles = defenseCannon.getProjectiles();
        for (int i = static_cast<int>(projectiles.size()) - 1; i >= 0; --i) {
            const size_t idx = static_cast<size_t>(i);
            if (playerRocket.isMoving() &&
                intersects(projectiles[idx].getPosition(),
                    playerRocket.getPosition(), 25.f)) {
                explosion.trigger(playerRocket.getPosition());
                projectiles.erase(projectiles.begin() + i);
                rocketHealth--;

                if (rocketHealth <= 0) {
                    score += 500;
                    currentState = State::GameOver;
                    break;
                }
            }
        }

        predictionSystem.update(rocketPositionsHistory, deltaTime);
        explosion.update(deltaTime);

        if (!playerRocket.isMoving() && rocketHealth > 0) {
            currentState = State::GameOver;
        }
    }
}

void Game::render() {
    window.clear(sf::Color(30, 30, 60));

    if (currentState == State::SelectingTrajectory) {
        // Отрисовка фона
        sf::RectangleShape background(sf::Vector2f(1024, 768));
        background.setFillColor(sf::Color(20, 20, 40));
        window.draw(background);

        // Отрисовка превью траекторий
        if (!availableTrajectories.empty()) {
            const int previewPoints = 100;
            sf::VertexArray preview(sf::LineStrip, previewPoints);
            for (int i = 0; i < previewPoints; ++i) {
                float t = static_cast<float>(i) / (previewPoints - 1);
                preview[i].position = availableTrajectories[selectedTrajectory].function(t);
                preview[i].color = sf::Color::Green;
            }
            window.draw(preview);
        }

        if (font.getInfo().family != "") {
            // Название траектории
            sf::Text nameText;
            nameText.setFont(font);
            nameText.setCharacterSize(36);
            nameText.setFillColor(sf::Color::Yellow);
            nameText.setPosition(400.f, 100.f);
            nameText.setString("Trajectory: " + availableTrajectories[selectedTrajectory].name);
            nameText.setOrigin(nameText.getLocalBounds().width / 2, 0);
            window.draw(nameText);

            // Инструкция
            sf::Text infoText;
            infoText.setFont(font);
            infoText.setCharacterSize(24);
            infoText.setFillColor(sf::Color::White);
            infoText.setPosition(400.f, 150.f);
            infoText.setString(
                "Press LEFT/RIGHT to change trajectory\n"
                "Press ENTER to launch rocket"
            );
            infoText.setOrigin(infoText.getLocalBounds().width / 2, 0);
            window.draw(infoText);

            // Счет
            sf::Text scoreText;
            scoreText.setFont(font);
            scoreText.setCharacterSize(30);
            scoreText.setFillColor(sf::Color::Cyan);
            scoreText.setPosition(900.f, 20.f);
            scoreText.setString("Score: " + std::to_string(score));
            window.draw(scoreText);
        }
    }
    else if (currentState == State::DefensePhase || currentState == State::GameOver) {
        defenseCannon.draw(window);
        playerRocket.draw(window);

        if (currentState == State::DefensePhase) {
            predictionSystem.draw(window);

            // Отрисовка луча из пушки
            sf::Vector2f barrelPos = defenseCannon.getBarrelPosition();
            float angle = defenseCannon.getRotation() * 3.14159f / 180.f;
            sf::Vector2f endPos = barrelPos + sf::Vector2f(
                std::cos(angle) * 2000.f,
                std::sin(angle) * 2000.f
            );

            sf::VertexArray laser(sf::Lines, 2);
            laser[0].position = barrelPos;
            laser[0].color = sf::Color::Red;
            laser[1].position = endPos;
            laser[1].color = sf::Color(255, 0, 0, 100);
            window.draw(laser);
        }

        explosion.draw(window);

        if (font.getInfo().family != "") {
            // Информация о здоровье и управлении
            sf::Text infoText;
            infoText.setFont(font);
            infoText.setCharacterSize(20);
            infoText.setFillColor(sf::Color::White);
            infoText.setPosition(10.f, 10.f);
            infoText.setString(
                "SCORE: " + std::to_string(score) + "\n" +
                "Rocket Health: " + std::to_string(rocketHealth) + "\n" +
                "Controls:\nLEFT/RIGHT - Rotate cannon\nSPACE - Fire"
            );
            window.draw(infoText);

            // Счет в углу
            sf::Text scoreText;
            scoreText.setFont(font);
            scoreText.setCharacterSize(30);
            scoreText.setFillColor(sf::Color::Cyan);
            scoreText.setPosition(900.f, 20.f);
            scoreText.setString("Score: " + std::to_string(score));
            window.draw(scoreText);
        }

        if (currentState == State::GameOver) {
            sf::RectangleShape overlay(sf::Vector2f(1024, 768));
            overlay.setFillColor(sf::Color(0, 0, 0, 150));
            window.draw(overlay);

            sf::Text gameOverText;
            gameOverText.setFont(font);
            gameOverText.setCharacterSize(40);
            gameOverText.setFillColor(rocketHealth <= 0 ? sf::Color::Green : sf::Color::Red);
            gameOverText.setPosition(512.f, 300.f);

            std::string result = rocketHealth <= 0 ?
                "ROCKET DESTROYED!\n+500 points" :
                "MISSION FAILED!\nRocket escaped";
            gameOverText.setString(result + "\n\nPress ENTER to restart");

            sf::FloatRect textRect = gameOverText.getLocalBounds();
            gameOverText.setOrigin(textRect.width / 2, textRect.height / 2);
            window.draw(gameOverText);
        }
    }

    window.display();
}

bool Game::intersects(const sf::Vector2f& a, const sf::Vector2f& b, float radius) const {
    return std::hypot(a.x - b.x, a.y - b.y) <= radius;
}