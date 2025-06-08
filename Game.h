#pragma once
#include "Cannon.h"
#include "Rocket.h"
#include "PredictionSystem.h"
#include "Explosion.h"
#include <vector>
#include <string>

class Game {
public:
    enum class State {
        SelectingTrajectory,
        DefensePhase,
        GameOver
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
    bool intersects(const sf::Vector2f& a, const sf::Vector2f& b, float radius) const;

    sf::RenderWindow window;
    Cannon defenseCannon;
    Rocket playerRocket;
    Explosion explosion;
    PredictionSystem predictionSystem;
    State currentState;
    int selectedTrajectory;
    int score;
    int rocketHealth;
    const size_t MAX_HISTORY = 50;
    std::vector<sf::Vector2f> rocketPositionsHistory;
    std::vector<Trajectory> availableTrajectories;
    sf::Font font;
    sf::Clock clock;
};