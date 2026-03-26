#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <memory>
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Collision.h"
#include "PowerUp.h"
#include "Particle.h"
#include "StarBackground.h"
#include "SoundManager.h"

// Game states enumeration
enum class GameState {
    START,
    PLAYING,
    LEVEL_COMPLETE,
    GAME_OVER,
    WIN
};

// Level system
enum class Level {
    LEVEL_1,
    LEVEL_2,
    LEVEL_3
};

struct LevelConfig {
    int targetScore;
    int enemyCount;
    float enemySpawnRate;
    float enemySpeed;
    std::string playerImage;
    std::string enemyImage;
    std::string bossImage;
    std::string bgColor;      // describes level theme for nebula
};

class Game {
private:
    sf::RenderWindow window;
    GameState currentState;
    Level currentLevel;
    Player player;
    std::vector<Enemy> enemies;
    std::vector<Bullet> bullets;
    sf::Font font;
    sf::Font emojiFont;        // Segoe UI Emoji for emoji rendering

    // HUD elements
    sf::Text scoreText;
    sf::Text healthLabel;
    sf::Text levelText;
    sf::Text targetText;
    sf::Text gameStateText;
    sf::Text subtitleText;
    sf::Text controlsText;
    sf::Text powerUpStatus;   // shows active power-up timer
    sf::Text fpsText;

    // Health bar
    sf::RectangleShape healthBarBg;
    sf::RectangleShape healthBarFg;
    sf::RectangleShape healthBarBorder;

    // HUD panel background
    sf::RectangleShape hudPanel;

    // Level management
    std::map<Level, LevelConfig> levelConfigs;
    int score;
    int targetScore;
    float enemySpawnTimer;
    float enemySpawnInterval;
    float difficultyMultiplier;

    // Power-up system
    std::vector<PowerUp> powerUps;
    float powerUpSpawnChance;
    float rapidFireTimer;
    bool hasRapidFire;

    // Sound
    SoundManager soundManager;

    // Visual effects
    ParticleSystem particles;
    std::unique_ptr<StarBackground> starBg;

    // Screen shake
    float shakeTimer;
    float shakeMagnitude;
    sf::Vector2f cameraOffset;

    // Animation timers
    float animTimer;
    float titlePulse;

    // FPS tracking
    float fpsTimer;
    int frameCount;
    float currentFPS;

    // Nebula overlay (colored gradient layer per level)
    sf::RectangleShape nebulaLayer;

    void initializeLevels();
    void setupLevel(Level level);
    void nextLevel();
    void checkLevelComplete();

    // Power-up methods
    void spawnPowerUp(float x, float y);
    void updatePowerUps(float deltaTime);
    void handlePowerUpCollisions();
    void applyPowerUpEffect(PowerUpType type);

    // Private methods
    void processEvents();
    void update(float deltaTime);
    void render();
    void spawnEnemy();
    void handleCollisions();
    void updateUI();
    void setupUI();
    void checkForCustomImages();
    void renderHUD();
    void renderStartScreen();
    void renderGameOverScreen();
    void renderLevelComplete();
    void renderWinScreen();
    void triggerScreenShake(float magnitude, float duration);
    void updateScreenShake(float dt);
    void drawHealthBar();

public:
    Game();
    void run();

    // Level management
    void setLevel(Level level);
    Level getCurrentLevel() const;
    int getTargetScore() const;
    bool isLevelComplete() const;
};

#endif // GAME_H
