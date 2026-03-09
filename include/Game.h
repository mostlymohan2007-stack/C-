#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Collision.h"

// Game states enumeration
enum class GameState {
    START,
    PLAYING,
    GAME_OVER
};

class Game {
private:
    sf::RenderWindow window;
    GameState currentState;
    Player player;
    std::vector<Enemy> enemies;
    std::vector<Bullet> bullets;
    sf::Font font;
    sf::Text scoreText;
    sf::Text healthText;
    sf::Text gameStateText;
    
    // Game variables
    int score;
    float enemySpawnTimer;
    float enemySpawnInterval;
    float difficultyTimer;
    
    // Private methods
    void processEvents();
    void update(float deltaTime);
    void render();
    void spawnEnemy();
    void handleCollisions();
    void updateUI();
    void increaseDifficulty();
    void resetGame();
    void setupUI();
    void showUploadInstructions();
    void checkForCustomImages();
    
public:
    Game();
    void run();
};

#endif // GAME_H
