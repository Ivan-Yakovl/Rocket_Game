#pragma once
#include <SFML/Audio.hpp>
#include "Cannon.h"
#include "RocketLauncher.h"
#include "Rocket.h"
#include "PredictionSystem.h"
#include "Explosion.h"
#include <vector>
#include <string>

class Game {
public:
    enum class State {
        SelectingTrajectory,
        SelectingWeapon,
        DefensePhase,
        GameOver
    };

    enum class WeaponType {
        Cannon,
        RocketLauncher
    };

    struct Trajectory {
        std::function<sf::Vector2f(float)> function;
        std::string name;
    };

    Game();
    void run();

private:
    void processEvents();
    void update(float deltaTime);
    void render();
    void restartGame();
    void initTrajectories();
    void initSounds();
    void playNextTrack();
    void updateMusic();
    bool intersects(const sf::Vector2f& a, const sf::Vector2f& b, float radius) const;
    void playSound(sf::Sound& sound);

    sf::RenderWindow window;
    Cannon defenseCannon;
    RocketLauncher rocketLauncher;
    Rocket playerRocket;
    Explosion explosion;
    PredictionSystem predictionSystem;
    State currentState;
    WeaponType selectedWeapon;
    int selectedTrajectory;
    int score;
    int rocketHealth;
    const size_t MAX_HISTORY = 50;
    std::vector<sf::Vector2f> rocketPositionsHistory;
    std::vector<Trajectory> availableTrajectories;
    sf::Font font;
    sf::Clock clock;

    bool rocketControlEnabled = false;
    float rocketControlDelay = 0.5f; 
    float rocketControlTimer = 0.1f;


    // Музыка
    sf::Music bgMusic[3];
    int currentTrack;
};