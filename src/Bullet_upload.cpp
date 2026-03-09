#include "Bullet.h"
#include <iostream>
#include <fstream>

Bullet::Bullet(BulletType type, float x, float y, float dirX, float dirY) 
    : type(type), active(true), damage(10) {
    
    // Set bullet properties based on type
    switch (type) {
        case BulletType::PLAYER:
            speed = 500.0f;
            damage = 10;
            break;
        case BulletType::ENEMY:
            speed = 300.0f;
            damage = 15;
            break;
    }
    
    direction.x = dirX;
    direction.y = dirY;
    
    // Try to load user-uploaded bullet textures
    loadUserBulletTextures();
    
    // If no user textures found, create colored fallback
    if (!texture.loadFromFile("assets/bullet.png")) {
        createFallbackBulletTexture();
    }
    
    sprite.setTexture(texture);
    sprite.setPosition(x, y);
    sprite.setOrigin(sprite.getLocalBounds().width / 2.0f, sprite.getLocalBounds().height / 2.0f);
}

void Bullet::loadUserBulletTextures() {
    // Check for user-uploaded bullet textures
    std::vector<std::string> userFiles = {
        "user_bullet.png",
        "my_bullet.png",
        "custom_bullet.png",
        "laser.png",
        "plasma.png",
        "shot.png"
    };
    
    for (const auto& filename : userFiles) {
        if (texture.loadFromFile(filename)) {
            std::cout << "Loaded user bullet texture: " << filename << std::endl;
            return;
        }
    }
}

void Bullet::createFallbackBulletTexture() {
    // Create different colored bullets for player vs enemy
    texture.create(5, 10);
    sf::Image image = texture.copyToImage();
    
    sf::Color bulletColor = (type == BulletType::PLAYER) ? sf::Color::Cyan : sf::Color::White;
    
    // Create a simple bullet shape
    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 5; x++) {
            if (x >= 1 && x <= 3) {
                image.setPixel(x, y, bulletColor);
            }
        }
    }
    
    texture.update(image);
}

void Bullet::promptUserUpload() {
    std::cout << "\n=== CUSTOMIZE BULLETS ===\n";
    std::cout << "To use your own bullet images:\n";
    std::cout << "1. Copy your images to game folder\n";
    std::cout << "2. Rename them to:\n";
    std::cout << "   - user_bullet.png (for all bullets)\n";
    std::cout << "   - or laser.png, plasma.png, shot.png\n";
    std::cout << "3. Recommended size: 5x10 pixels\n";
    std::cout << "4. Supported formats: PNG, JPG, BMP, TGA\n";
    std::cout << "5. Restart the game to load your images\n\n";
    std::cout << "Press Enter to continue...";
    std::cin.ignore();
}
