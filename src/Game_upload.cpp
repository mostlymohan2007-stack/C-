#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <filesystem>

Game::Game() : window(sf::VideoMode(800, 600), "Space Battle Game"), 
               currentState(GameState::START), score(0), 
               enemySpawnTimer(0.0f), enemySpawnInterval(2.0f), difficultyTimer(0.0f) {
    
    srand(static_cast<unsigned int>(time(nullptr)));
    
    // Load font
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        if (!font.loadFromFile("C:/Windows/Fonts/calibri.ttf")) {
            std::cout << "Warning: Could not load default font." << std::endl;
        }
    }
    
    setupUI();
    player.setPosition(400.0f, 500.0f);
    window.setFramerateLimit(60);
    
    // Show upload instructions on first run
    showUploadInstructions();
}

void Game::setupUI() {
    scoreText.setFont(font);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);
    
    healthText.setFont(font);
    healthText.setCharacterSize(20);
    healthText.setFillColor(sf::Color::White);
    healthText.setPosition(10, 40);
    
    gameStateText.setFont(font);
    gameStateText.setCharacterSize(40);
    gameStateText.setFillColor(sf::Color::White);
    gameStateText.setPosition(200, 250);
}

void Game::showUploadInstructions() {
    std::cout << "\n========================================\n";
    std::cout << "   SPACE BATTLE - CUSTOMIZE YOUR GAME\n";
    std::cout << "========================================\n\n";
    
    std::cout << "🎮 HOW TO ADD CUSTOM IMAGES:\n\n";
    
    std::cout << "🚀 PLAYER SHIP:\n";
    std::cout << "   Copy your image to game folder\n";
    std::cout << "   Rename to: user_player.png\n";
    std::cout << "   Size: 40x40 pixels\n\n";
    
    std::cout << "👾 ENEMY SHIPS:\n";
    std::cout << "   Copy your image to game folder\n";
    std::cout << "   Rename to: user_enemy.png\n";
    std::cout << "   Size: 30x30 pixels\n\n";
    
    std::cout << "⚡ BULLETS:\n";
    std::cout << "   Copy your image to game folder\n";
    std::cout << "   Rename to: user_bullet.png\n";
    std::cout << "   Size: 5x10 pixels\n\n";
    
    std::cout << "📁 SUPPORTED NAMES:\n";
    std::cout << "   Player: user_player.png, my_ship.png, custom_player.png\n";
    std::cout << "   Enemy: user_enemy.png, alien.png, invader.png\n";
    std::cout << "   Bullet: user_bullet.png, laser.png, plasma.png\n\n";
    
    std::cout << "🎨 FORMATS: PNG, JPG, BMP, TGA\n";
    std::cout << "⚠️  RESTART game after adding images\n\n";
    
    std::cout << "Press Enter to start game...";
    std::cin.ignore();
    
    // Check for existing custom images
    checkForCustomImages();
}

void Game::checkForCustomImages() {
    std::cout << "\n🔍 CHECKING FOR CUSTOM IMAGES...\n";
    
    std::vector<std::pair<std::string, std::string>> imageTypes = {
        {"Player", "user_player.png"},
        {"Player", "my_ship.png"},
        {"Enemy", "user_enemy.png"},
        {"Enemy", "alien.png"},
        {"Bullet", "user_bullet.png"},
        {"Bullet", "laser.png"}
    };
    
    bool foundAny = false;
    for (const auto& [type, filename] : imageTypes) {
        if (std::filesystem::exists(filename)) {
            std::cout << "✅ Found custom " << type << ": " << filename << std::endl;
            foundAny = true;
        }
    }
    
    if (!foundAny) {
        std::cout << "ℹ️  No custom images found. Using default graphics.\n";
    }
    
    std::cout << "\n";
}

void Game::run() {
    sf::Clock clock;
    
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        
        processEvents();
        
        if (currentState == GameState::PLAYING) {
            update(deltaTime);
        }
        
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
                else if (currentState == GameState::START) {
                    if (event.key.code == sf::Keyboard::Space) {
                        currentState = GameState::PLAYING;
                        resetGame();
                    }
                }
                else if (currentState == GameState::GAME_OVER) {
                    if (event.key.code == sf::Keyboard::Space) {
                        currentState = GameState::PLAYING;
                        resetGame();
                    }
                }
                break;
        }
    }
    
    if (currentState == GameState::PLAYING) {
        float moveSpeed = 300.0f * 0.016f;
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            player.move(-moveSpeed, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            player.move(moveSpeed, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            player.move(0, -moveSpeed);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            player.move(0, moveSpeed);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            if (player.canShoot()) {
                sf::Vector2f playerPos = player.getPosition();
                bullets.emplace_back(BulletType::PLAYER, playerPos.x, playerPos.y - 20, 0, -1);
                player.resetFireTimer();
            }
        }
    }
}

void Game::update(float deltaTime) {
    player.update(deltaTime);
    
    for (auto& enemy : enemies) {
        enemy.update(deltaTime);
        
        if (enemy.shouldShoot()) {
            sf::Vector2f enemyPos = enemy.getPosition();
            bullets.emplace_back(BulletType::ENEMY, enemyPos.x, enemyPos.y + 20, 0, 1);
            enemy.resetFireTimer();
        }
    }
    
    for (auto& bullet : bullets) {
        bullet.update(deltaTime);
    }
    
    enemySpawnTimer += deltaTime;
    if (enemySpawnTimer >= enemySpawnInterval) {
        spawnEnemy();
        enemySpawnTimer = 0.0f;
    }
    
    difficultyTimer += deltaTime;
    if (difficultyTimer >= 10.0f) {
        increaseDifficulty();
        difficultyTimer = 0.0f;
    }
    
    handleCollisions();
    
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [](const Enemy& e) { return !e.isAlive(); }), enemies.end());
    
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [this](const Bullet& b) { return !b.isActive() || b.isOutOfBounds(800, 600); }), bullets.end());
    
    if (!player.isAlive()) {
        currentState = GameState::GAME_OVER;
    }
    
    updateUI();
}

void Game::render() {
    window.clear(sf::Color::Black);
    
    if (currentState == GameState::START) {
        gameStateText.setString("SPACE BATTLE\n\nPress SPACE to Start\n\nArrow Keys: Move\nSpace: Shoot\nESC: Exit");
        window.draw(gameStateText);
    }
    else if (currentState == GameState::PLAYING) {
        player.render(window);
        
        for (auto& enemy : enemies) {
            enemy.render(window);
        }
        
        for (auto& bullet : bullets) {
            bullet.render(window);
        }
        
        window.draw(scoreText);
        window.draw(healthText);
    }
    else if (currentState == GameState::GAME_OVER) {
        gameStateText.setString("GAME OVER\n\nScore: " + std::to_string(score) + 
                               "\n\nPress SPACE to Play Again\nESC to Exit");
        window.draw(gameStateText);
    }
    
    window.display();
}

void Game::spawnEnemy() {
    float x = rand() % 700 + 50;
    float y = -50;
    
    EnemyType type;
    int randType = rand() % 100;
    if (enemySpawnInterval < 1.0f) {
        if (randType < 20) type = EnemyType::HEAVY;
        else if (randType < 60) type = EnemyType::FAST;
        else type = EnemyType::BASIC;
    }
    else if (enemySpawnInterval < 1.5f) {
        if (randType < 10) type = EnemyType::HEAVY;
        else if (randType < 40) type = EnemyType::FAST;
        else type = EnemyType::BASIC;
    }
    else {
        if (randType < 5) type = EnemyType::HEAVY;
        else if (randType < 20) type = EnemyType::FAST;
        else type = EnemyType::BASIC;
    }
    
    enemies.emplace_back(type, x, y);
}

void Game::handleCollisions() {
    for (auto& enemy : enemies) {
        if (Collision::checkSpriteCollision(player.getSprite(), enemy.getSprite())) {
            player.takeDamage(20);
            enemy.takeDamage(enemy.getHealth());
        }
    }
    
    for (auto& bullet : bullets) {
        if (bullet.getType() == BulletType::PLAYER) {
            for (auto& enemy : enemies) {
                if (Collision::checkBulletSpriteCollision(bullet, enemy.getSprite())) {
                    enemy.takeDamage(bullet.getDamage());
                    bullet.setActive(false);
                    
                    if (!enemy.isAlive()) {
                        score += enemy.getScoreValue();
                    }
                }
            }
        }
    }
    
    for (auto& bullet : bullets) {
        if (bullet.getType() == BulletType::ENEMY) {
            if (Collision::checkBulletSpriteCollision(bullet, player.getSprite())) {
                player.takeDamage(bullet.getDamage());
                bullet.setActive(false);
            }
        }
    }
}

void Game::updateUI() {
    scoreText.setString("Score: " + std::to_string(score));
    healthText.setString("Health: " + std::to_string(player.getHealth()) + "/" + 
                       std::to_string(player.getMaxHealth()));
}

void Game::increaseDifficulty() {
    if (enemySpawnInterval > 0.5f) {
        enemySpawnInterval -= 0.2f;
    }
}

void Game::resetGame() {
    score = 0;
    enemySpawnTimer = 0.0f;
    enemySpawnInterval = 2.0f;
    difficultyTimer = 0.0f;
    
    player.setPosition(400.0f, 500.0f);
    player.heal(player.getMaxHealth());
    
    enemies.clear();
    bullets.clear();
}
