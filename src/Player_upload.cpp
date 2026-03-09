#include "Player.h"
#include <iostream>
#include <fstream>

Player::Player() : speed(300.0f), health(100), maxHealth(100), fireRate(0.2f), fireTimer(0.0f) {
    // Try to load user-uploaded textures first
    loadUserTextures();
    
    // If no user textures found, create colored fallback
    if (!texture.loadFromFile("assets/player.png")) {
        createFallbackTexture();
    }
    
    sprite.setTexture(texture);
    sprite.setPosition(400.0f, 500.0f);
    sprite.setOrigin(sprite.getLocalBounds().width / 2.0f, sprite.getLocalBounds().height / 2.0f);
}

void Player::loadUserTextures() {
    // Check for user-uploaded textures in current directory
    std::vector<std::string> userFiles = {
        "user_player.png",
        "my_ship.png", 
        "custom_player.png",
        "player_custom.png"
    };
    
    for (const auto& filename : userFiles) {
        if (texture.loadFromFile(filename)) {
            std::cout << "Loaded user texture: " << filename << std::endl;
            return;
        }
    }
    
    std::cout << "No user texture found. Using default or fallback." << std::endl;
}

void Player::createFallbackTexture() {
    // Create a simple spaceship shape as fallback
    texture.create(40, 40);
    sf::Image image = texture.copyToImage();
    
    // Create a triangle shape for spaceship
    for (int y = 0; y < 40; y++) {
        for (int x = 0; x < 40; x++) {
            // Create triangle pointing upward
            if (y >= 30 && x >= 15 && x <= 25) {
                image.setPixel(x, y, sf::Color::Blue);
            }
            else if (y >= 20 && y < 30 && x >= 10 && x <= 30) {
                image.setPixel(x, y, sf::Color::Blue);
            }
            else if (y >= 10 && y < 20 && x >= 18 && x <= 22) {
                image.setPixel(x, y, sf::Color::Blue);
            }
        }
    }
    
    texture.update(image);
}

void Player::promptUserUpload() {
    std::cout << "\n=== CUSTOMIZE YOUR SPACESHIP ===\n";
    std::cout << "To use your own spaceship image:\n";
    std::cout << "1. Copy your image to game folder\n";
    std::cout << "2. Rename it to: user_player.png\n";
    std::cout << "3. Recommended size: 40x40 pixels\n";
    std::cout << "4. Supported formats: PNG, JPG, BMP, TGA\n";
    std::cout << "5. Restart the game to load your image\n\n";
    std::cout << "Press Enter to continue...";
    std::cin.ignore();
}

void Player::move(float offsetX, float offsetY) {
    sprite.move(offsetX, offsetY);
}

void Player::update(float deltaTime) {
    if (fireTimer > 0.0f) {
        fireTimer -= deltaTime;
    }
}

bool Player::canShoot() {
    return fireTimer <= 0.0f;
}

void Player::resetFireTimer() {
    fireTimer = fireRate;
}

void Player::takeDamage(int damage) {
    health -= damage;
    if (health < 0) health = 0;
}

void Player::heal(int amount) {
    health += amount;
    if (health > maxHealth) health = maxHealth;
}

int Player::getHealth() const {
    return health;
}

int Player::getMaxHealth() const {
    return maxHealth;
}

bool Player::isAlive() const {
    return health > 0;
}

sf::Vector2f Player::getPosition() const {
    return sprite.getPosition();
}

void Player::setPosition(float x, float y) {
    sprite.setPosition(x, y);
}

void Player::render(sf::RenderWindow& window) {
    window.draw(sprite);
}

sf::FloatRect Player::getBounds() const {
    return sprite.getGlobalBounds();
}

sf::Sprite Player::getSprite() const {
    return sprite;
}
