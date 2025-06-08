#include "Game.h"
#include <algorithm>
#include <iostream>

#pragma warning(push)
#pragma warning(disable: 26495)
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#pragma warning(pop)
Game::Game() :
    window(sf::VideoMode(1024, 768), "Rocket Defense"),
    defenseCannon(sf::Vector2f(100.f, 700.f)),
    rocketLauncher(sf::Vector2f(100.f, 700.f)),
    currentState(State::SelectingTrajectory),
    selectedWeapon(WeaponType::Cannon),
    selectedTrajectory(0),
    score(0),
    rocketHealth(2)
{
    window.setVerticalSyncEnabled(true);

    if (!font.loadFromFile("arial.ttf")) {
        font.loadFromFile("C:/Windows/Fonts/arial.ttf");
    }

    initTrajectories();
    initSounds();
}

void Game::run() {
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        processEvents();
        update(deltaTime);
        render();
    }
}

void Game::playSound(sf::Sound& sound) {
    if (sound.getStatus() != sf::Sound::Playing) {
        sound.play();
    }
}

void Game::initTrajectories() {
    availableTrajectories.clear();

    // Синусоида (смещена вниз на 100 пикселей)
    availableTrajectories.push_back({
        [](float t) -> sf::Vector2f {
            return sf::Vector2f(
                100.f + t * 900.f,
                400.f + 80.f * std::sin(t * 3.14f * 4.f)  // Было 300.f, стало 400.f
            );
        },
        "Sinusoid"
        });

    // Парабола (смещена вниз на 100 пикселей)
    availableTrajectories.push_back({
        [](float t) -> sf::Vector2f {
            return sf::Vector2f(
                100.f + t * 900.f,
                600.f - 300.f * t * (1.f - t)  // Было 500.f, стало 600.f
            );
        },
        "Parabola"
        });

    // Прямая (смещена вниз на 100 пикселей)
    availableTrajectories.push_back({
        [](float t) -> sf::Vector2f {
            return sf::Vector2f(
                100.f + t * 900.f,
                300.f + t * 300.f  // Было 200.f, стало 300.f
            );
        },
        "Straight Line"
        });

    // Квадратичная (смещена вниз на 100 пикселей)
    availableTrajectories.push_back({
        [](float t) -> sf::Vector2f {
            float offset = t - 0.5f;
            return sf::Vector2f(
                100.f + t * 900.f,
                500.f - 200.f * (offset * offset) * 4.f  // Было 400.f, стало 500.f
            );
        },
        "Quadratic"
        });

    // Спираль (смещена вниз на 100 пикселей)
    availableTrajectories.push_back({
        [](float t) -> sf::Vector2f {
            return sf::Vector2f(
                100.f + t * 900.f,
                400.f + 150.f * std::sin(t * 3.14f * 6.f) * (1 - t)  // Было 300.f, стало 400.f
            );
        },
        "Spiral"
        });
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        if (event.type == sf::Event::KeyPressed) {
            if (currentState == State::SelectingTrajectory) {
                if (event.key.code == sf::Keyboard::Left) {
                    selectedTrajectory = (selectedTrajectory - 1 + static_cast<int>(availableTrajectories.size())) % static_cast<int>(availableTrajectories.size());
                }
                else if (event.key.code == sf::Keyboard::Right) {
                    selectedTrajectory = (selectedTrajectory + 1) % static_cast<int>(availableTrajectories.size());
                }
                else if (event.key.code == sf::Keyboard::Enter && !availableTrajectories.empty()) {
                    currentState = State::SelectingWeapon;
                }
            }
            else if (currentState == State::SelectingWeapon) {
                if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right) {
                    selectedWeapon = (selectedWeapon == WeaponType::Cannon) ? WeaponType::RocketLauncher : WeaponType::Cannon;
                }
                else if (event.key.code == sf::Keyboard::Enter) {
                    playerRocket.setTrajectory(availableTrajectories[selectedTrajectory].function, 7.f);
                    rocketHealth = (selectedWeapon == WeaponType::RocketLauncher) ? 1 : 2;
                    currentState = State::DefensePhase;
                    rocketPositionsHistory.clear();
                }
            }
            else if (currentState == State::DefensePhase) {
                if (event.key.code == sf::Keyboard::Space) {
                    if (selectedWeapon == WeaponType::Cannon) {
                        defenseCannon.fire();
                    }
                    else if (rocketLauncher.isRocketActive()) {
                        rocketLauncher.explode();
                        explosion.trigger(rocketLauncher.getRocketPosition());
                    }
                    else {
                        rocketLauncher.fire();
                        rocketControlEnabled = false;
                        rocketControlTimer = 0.0f;
                    }
                }
            }
            else if (currentState == State::GameOver && event.key.code == sf::Keyboard::Enter) {
                restartGame();
            }
        }
    }
}
void Game::restartGame() {
    currentState = State::SelectingTrajectory;
    selectedTrajectory = 0;
    rocketHealth = (selectedWeapon == WeaponType::RocketLauncher) ? 1 : 2;
    rocketPositionsHistory.clear();
    predictionSystem.reset();
    explosion.reset();
    rocketLauncher.reset();
    playerRocket = Rocket(); // Полный сброс ракеты
}
void Game::initSounds() {
    // Абсолютный путь для отладки
    std::string basePath = "C:/Users/oktan/source/repos/RocketDefenseSim/x64/Debug/";

    const std::string tracks[3] = {
        basePath + "music/track1.wav",
        basePath + "music/track2.wav",
        basePath + "music/track3.wav"
    };

    for (int i = 0; i < 3; ++i) {
        if (!bgMusic[i].openFromFile(tracks[i])) {
            std::cerr << "ERROR: Music track " << i << " not found!" << std::endl;
        }
        bgMusic[i].setLoop(true);
        bgMusic[i].setVolume(50); // Уменьшите громкость для теста
    } 

    
    // Запуск музыки
    bgMusic[0].play();
}
void Game::playNextTrack() {
    bgMusic[currentTrack].stop();
    currentTrack = (currentTrack + 1) % 3;
    bgMusic[currentTrack].play();
}

void Game::updateMusic() {
    if (bgMusic[currentTrack].getStatus() != sf::Music::Playing) {
        playNextTrack();
    }
}

void Game::update(float deltaTime) {
    if (currentState == State::DefensePhase) {
        // Обновляем таймер управления ракетой
        if (!rocketControlEnabled && rocketLauncher.isRocketActive()) {
            rocketControlTimer += deltaTime;
            if (rocketControlTimer >= rocketControlDelay) {
                rocketControlEnabled = true;
            }
        }

        if (selectedWeapon == WeaponType::Cannon) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                defenseCannon.rotate(-60.f * deltaTime);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                defenseCannon.rotate(60.f * deltaTime);
            }

            defenseCannon.update(deltaTime);

            // Проверка попаданий снарядов
            auto& projectiles = defenseCannon.getProjectiles();
            for (int i = static_cast<int>(projectiles.size()) - 1; i >= 0; --i) {
                if (intersects(projectiles[i].getPosition(), playerRocket.getPosition(), 25.f)) {
                    explosion.trigger(playerRocket.getPosition());
                    projectiles.erase(projectiles.begin() + i);
                    rocketHealth--;

                    if (rocketHealth <= 0) {
                        score += 500;
                        currentState = State::GameOver;
                    }
                    break;
                }
            }
        }
        else if (selectedWeapon == WeaponType::RocketLauncher) {
            // Управление ракетой
            if (rocketLauncher.isRocketActive() && rocketControlEnabled) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                    rocketLauncher.rotateRocket(-180.f * deltaTime);
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                    rocketLauncher.rotateRocket(180.f * deltaTime);
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                    rocketLauncher.accelerateRocket(400.f * deltaTime);
                }
            }

            // Проверка попаданий
            if (rocketLauncher.isRocketActive() &&
                intersects(rocketLauncher.getRocketPosition(), playerRocket.getPosition(),
                    rocketLauncher.getRocketRadius() + 15.f)) { // Увеличенный радиус для лучшего попадания
                explosion.trigger(playerRocket.getPosition());
                rocketLauncher.explode();
                rocketHealth--;

                if (rocketHealth <= 0) {
                    score += 500;
                    currentState = State::GameOver;
                }
            }

            if (explosion.isActive() &&
                intersects(explosion.getPosition(), playerRocket.getPosition(), explosion.getRadius())) {
                rocketHealth--;

                if (rocketHealth <= 0) {
                    score += 500;
                    currentState = State::GameOver;
                }
                explosion.reset();
            }
        }

        rocketLauncher.update(deltaTime);
        playerRocket.update(deltaTime);

        if (playerRocket.isMoving()) {
            rocketPositionsHistory.push_back(playerRocket.getPosition());
            if (rocketPositionsHistory.size() > MAX_HISTORY) {
                rocketPositionsHistory.erase(rocketPositionsHistory.begin());
            }
        }

        // Упрощенный PredictionSystem (только одна линия)
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

            // Смещаем график вниз на 50 пикселей дополнительно для лучшего отображения
            for (int i = 0; i < previewPoints; ++i) {
                preview[i].position.y += 50;
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
                "Press ENTER to continue"
            );
            infoText.setOrigin(infoText.getLocalBounds().width / 2, 0);
            window.draw(infoText);
        }
    }
    else if (currentState == State::SelectingWeapon) {
        sf::RectangleShape background(sf::Vector2f(1024, 768));
        background.setFillColor(sf::Color(20, 20, 40));
        window.draw(background);

        sf::Text title;
        title.setFont(font);
        title.setCharacterSize(40);
        title.setFillColor(sf::Color::Yellow);
        title.setPosition(512.f, 100.f);
        title.setString("Select Weapon");
        title.setOrigin(title.getLocalBounds().width / 2, 0);
        window.draw(title);

        // Cannon option
        sf::Text cannonText;
        cannonText.setFont(font);
        cannonText.setCharacterSize(30);
        cannonText.setFillColor(selectedWeapon == WeaponType::Cannon ? sf::Color::Green : sf::Color::White);
        cannonText.setPosition(512.f, 200.f);
        cannonText.setString("1. Cannon (Rotating turret with direct fire)");
        cannonText.setOrigin(cannonText.getLocalBounds().width / 2, 0);
        window.draw(cannonText);

        // Rocket Launcher option
        sf::Text rlText;
        rlText.setFont(font);
        rlText.setCharacterSize(30);
        rlText.setFillColor(selectedWeapon == WeaponType::RocketLauncher ? sf::Color::Green : sf::Color::White);
        rlText.setPosition(512.f, 250.f);
        rlText.setString("2. Rocket Launcher (Guided missile)");
        rlText.setOrigin(rlText.getLocalBounds().width / 2, 0);
        window.draw(rlText);

        // Инструкция
        sf::Text infoText;
        infoText.setFont(font);
        infoText.setCharacterSize(24);
        infoText.setFillColor(sf::Color::White);
        infoText.setPosition(512.f, 350.f);
        infoText.setString(
            "Press LEFT/RIGHT to select weapon\n"
            "Press ENTER to confirm"
        );
        infoText.setOrigin(infoText.getLocalBounds().width / 2, 0);
        window.draw(infoText);
    }
    else if (currentState == State::DefensePhase || currentState == State::GameOver) {
        if (selectedWeapon == WeaponType::Cannon) {
            defenseCannon.draw(window);

            // Отрисовка луча направления
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
        else {
            rocketLauncher.draw(window);
        }
        playerRocket.draw(window);

        if (currentState == State::DefensePhase) {
            predictionSystem.draw(window);
        }

        explosion.draw(window);

        if (font.getInfo().family != "") {
            // Информация о здоровье и управлении
            sf::Text infoText;
            infoText.setFont(font);
            infoText.setCharacterSize(20);
            infoText.setFillColor(sf::Color::White);
            infoText.setPosition(10.f, 10.f);

            std::string weaponName = (selectedWeapon == WeaponType::Cannon) ?
                "Cannon" : "Rocket Launcher";

            std::string controls = (selectedWeapon == WeaponType::Cannon) ?
                "Controls:\nLEFT/RIGHT - Rotate cannon\nSPACE - Fire" :
                "Controls:\nLEFT/RIGHT - Rotate rocket\nUP - Accelerate\nSPACE - Fire/Explode";

            infoText.setString(
                "Weapon: " + weaponName + "\n" +
                "SCORE: " + std::to_string(score) + "\n" +
                "Rocket Health: " + std::to_string(rocketHealth) + "\n" +
                controls
            );
            window.draw(infoText);
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
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return (dx * dx + dy * dy) <= (radius * radius);
}