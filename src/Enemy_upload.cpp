#include "Enemy.h"
#include <iostream>
#include <fstream>

Enemy::Enemy(EnemyType type, float x, float y) : type(type), fireTimer(0.0f) {
    // Set enemy properties based on type
    switch (type) {
        case EnemyType::BASIC:
            speed = 100.0f;
            health = 30;
            maxHealth = 30;
            fireRate = 2.0f;
            shootChance = 0.01f;
            break;
        case EnemyType::FAST:
            speed = 200.0f;
            health = 20;
            maxHealth = 20;
            fireRate = 1.5f;
            shootChance = 0.02f;
            break;
        case EnemyType::HEAVY:
            speed = 50.0f;
            health = 50;
            maxHealth = 50;
            fireRate = 3.0f;
            shootChance = 0.015f;
            break;
    }
    
    // Try to load user-uploaded enemy textures
    loadUserEnemyTextures();
    
    // If no user textures found, create colored fallback
    if (!texture.loadFromFile("assets/enemy.png")) {
        createFallbackEnemyTexture();
    }
    
    sprite.setTexture(texture);
    sprite.setPosition(x, y);
    sprite.setOrigin(sprite.getLocalBounds().width / 2.0f, sprite.getLocalBounds().height / 2.0f);
    
    // Set initial movement direction (downward with slight random horizontal movement)
    direction.x = (rand() % 100 - 50) / 100.0f;
    direction.y = 1.0f;
}

void Enemy::loadUserEnemyTextures() {
    // Check for user-uploaded enemy textures
    std::vector<std::string> userFiles = {
        "user_enemy.png",
        "my_enemy.png",
        "custom_enemy.png",
        "enemy_custom.png",
        "alien.png",
        "invader.png"
    };
    
    for (const auto& filename : userFiles) {
        if (texture.loadFromFile(filename)) {
            std::cout << "Loaded user enemy texture: " << filename << std::endl;
            return;
        }
    }
}

void Enemy::createFallbackEnemyTexture() {
    // Create different colored shapes for different enemy types
    texture.create(30, 30);
    sf::Image image = texture.copyToImage();
    
    sf::Color enemyColor;
    switch (type) {
        case EnemyType::BASIC:
            enemyColor = sf::Color::Red;
            break;
        case EnemyType::FAST:
            enemyColor = sf::Color::Yellow;
            break;
        case EnemyType::HEAVY:
            enemyColor = sf::Color::Magenta;
            break;
    }
    
    // Create different shapes for different enemy types
    for (int y = 0; y < 30; y++) {
        for (int x = 0; x < 30; x++) {
            switch (type) {
                case EnemyType::BASIC:
                    // Diamond shape
                    if ((x == 15 && y >= 5 && y <= 25) ||
                        (y == 15 && x >= 5 && x <= 25)) {
                        image.setPixel(x, y, enemyColor);
                    }
                    break;
                case EnemyType::FAST:
                    // Triangle shape
                    if (y >= 20 && x >= 10 && x <= 20) {
                        image.setPixel(x, y, enemyColor);
                    }
                    break;
                case EnemyType::HEAVY:
                    // Square shape
                    if (x >= 5 && x <= 25 && y >= 5 && y <= 25) {
                        image.setPixel(x, y, enemyColor);
                    }
                    break;
            }
        }
    }
    
    texture.update(image);
}

void Enemy::promptUserUpload() {
    std::cout << "\n=== CUSTOMIZE ENEMY SHIPS ===\n";
    std::cout << "To use your own enemy images:\n";
    std::cout << "1. Copy your images to game folder\n";
    std::cout << "2. Rename them to:\n";
    std::cout << "   - user_enemy.png (for all enemies)\n";
    std::cout << "   - or alien.png, invader.png\n";
    std::cout << "3. Recommended size: 30x30 pixels\n";
    std::cout << "4. Supported formats: PNG, JPG, BMP, TGA\n";
    std::cout << "5. Restart the game to load your images\n\n";
    std::cout << "Press Enter to continue...";
    std::cin.ignore();
}
