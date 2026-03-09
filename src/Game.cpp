#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

Game::Game() : window(sf::VideoMode(800, 600), "Space Battle Game"), 
               currentState(GameState::START), score(0), 
               enemySpawnTimer(0.0f), enemySpawnInterval(2.0f), difficultyTimer(0.0f) {
    
    // Initialize random seed
    srand(static_cast<unsigned int>(time(nullptr)));
    
    // Load font
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        // Try alternative fonts if Arial is not available
        if (!font.loadFromFile("C:/Windows/Fonts/calibri.ttf")) {
            std::cout << "Warning: Could not load default font. UI text may not display." << std::endl;
        }
    }
    
    // Setup UI text
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
    
    // Initialize player
    player.setPosition(400.0f, 500.0f);
    
    window.setFramerateLimit(60); // Limit to 60 FPS
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
    
    // Handle continuous input during gameplay
    if (currentState == GameState::PLAYING) {
        float moveSpeed = 300.0f; // Player movement speed
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            player.move(-moveSpeed * 0.016f, 0); // Assuming 60 FPS
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            player.move(moveSpeed * 0.016f, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            player.move(0, -moveSpeed * 0.016f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            player.move(0, moveSpeed * 0.016f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            if (player.canShoot()) {
                // Create player bullet
                sf::Vector2f playerPos = player.getPosition();
                bullets.emplace_back(BulletType::PLAYER, playerPos.x, playerPos.y - 20, 0, -1);
                player.resetFireTimer();
            }
        }
    }
}

void Game::update(float deltaTime) {
    // Update player
    player.update(deltaTime);
    
    // Update enemies
    for (auto& enemy : enemies) {
        enemy.update(deltaTime);
        
        // Enemy shooting
        if (enemy.shouldShoot()) {
            sf::Vector2f enemyPos = enemy.getPosition();
            bullets.emplace_back(BulletType::ENEMY, enemyPos.x, enemyPos.y + 20, 0, 1);
            enemy.resetFireTimer();
        }
    }
    
    // Update bullets
    for (auto& bullet : bullets) {
        bullet.update(deltaTime);
    }
    
    // Spawn enemies
    enemySpawnTimer += deltaTime;
    if (enemySpawnTimer >= enemySpawnInterval) {
        spawnEnemy();
        enemySpawnTimer = 0.0f;
    }
    
    // Increase difficulty over time
    difficultyTimer += deltaTime;
    if (difficultyTimer >= 10.0f) { // Every 10 seconds
        increaseDifficulty();
        difficultyTimer = 0.0f;
    }
    
    // Handle collisions
    handleCollisions();
    
    // Remove dead enemies and inactive bullets
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [](const Enemy& e) { return !e.isAlive(); }), enemies.end());
    
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [this](const Bullet& b) { return !b.isActive() || b.isOutOfBounds(800, 600); }), bullets.end());
    
    // Check game over condition
    if (!player.isAlive()) {
        currentState = GameState::GAME_OVER;
    }
    
    // Update UI
    updateUI();
}

void Game::render() {
    window.clear(sf::Color::Black);
    
    if (currentState == GameState::START) {
        gameStateText.setString("SPACE BATTLE\n\nPress SPACE to Start\n\nArrow Keys: Move\nSpace: Shoot\nESC: Exit");
        window.draw(gameStateText);
    }
    else if (currentState == GameState::PLAYING) {
        // Render game objects
        player.render(window);
        
        for (auto& enemy : enemies) {
            enemy.render(window);
        }
        
        for (auto& bullet : bullets) {
            bullet.render(window);
        }
        
        // Render UI
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
    // Random position at top of screen
    float x = rand() % 700 + 50; // Between 50 and 750
    float y = -50; // Start above screen
    
    // Random enemy type based on difficulty
    EnemyType type;
    int randType = rand() % 100;
    if (enemySpawnInterval < 1.0f) { // High difficulty
        if (randType < 20) type = EnemyType::HEAVY;
        else if (randType < 60) type = EnemyType::FAST;
        else type = EnemyType::BASIC;
    }
    else if (enemySpawnInterval < 1.5f) { // Medium difficulty
        if (randType < 10) type = EnemyType::HEAVY;
        else if (randType < 40) type = EnemyType::FAST;
        else type = EnemyType::BASIC;
    }
    else { // Low difficulty
        if (randType < 5) type = EnemyType::HEAVY;
        else if (randType < 20) type = EnemyType::FAST;
        else type = EnemyType::BASIC;
    }
    
    enemies.emplace_back(type, x, y);
}

void Game::handleCollisions() {
    // Player-Enemy collisions
    for (auto& enemy : enemies) {
        if (Collision::checkSpriteCollision(player.getSprite(), enemy.getSprite())) {
            player.takeDamage(20);
            enemy.takeDamage(enemy.getHealth()); // Destroy enemy on collision
        }
    }
    
    // Bullet-Enemy collisions
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
    
    // Bullet-Player collisions
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
    // Increase spawn rate
    if (enemySpawnInterval > 0.5f) {
        enemySpawnInterval -= 0.2f;
    }
}

void Game::resetGame() {
    score = 0;
    enemySpawnTimer = 0.0f;
    enemySpawnInterval = 2.0f;
    difficultyTimer = 0.0f;
    
    // Reset player
    player.setPosition(400.0f, 500.0f);
    player.heal(player.getMaxHealth()); // Full health
    
    // Clear enemies and bullets
    enemies.clear();
    bullets.clear();
}
