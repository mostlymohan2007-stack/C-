#include "Bullet.h"

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
    
    // Load bullet texture
    if (!texture.loadFromFile("assets/bullet.png")) {
        // Create a simple colored rectangle as fallback
        texture.create(5, 10);
        sf::Image image = texture.copyToImage();
        switch (type) {
            case BulletType::PLAYER:
                image.create(5, 10, sf::Color::Cyan);
                break;
            case BulletType::ENEMY:
                image.create(5, 10, sf::Color::White);
                break;
        }
        texture.update(image);
    }
    
    sprite.setTexture(texture);
    sprite.setPosition(x, y);
    sprite.setOrigin(sprite.getLocalBounds().width / 2.0f, sprite.getLocalBounds().height / 2.0f);
}

void Bullet::update(float deltaTime) {
    // Move the bullet
    sprite.move(direction.x * speed * deltaTime, direction.y * speed * deltaTime);
}

bool Bullet::isActive() const {
    return active;
}

void Bullet::setActive(bool active) {
    this->active = active;
}

BulletType Bullet::getType() const {
    return type;
}

int Bullet::getDamage() const {
    return damage;
}

sf::Vector2f Bullet::getPosition() const {
    return sprite.getPosition();
}

void Bullet::setPosition(float x, float y) {
    sprite.setPosition(x, y);
}

void Bullet::render(sf::RenderWindow& window) {
    if (active) {
        window.draw(sprite);
    }
}

sf::FloatRect Bullet::getBounds() const {
    return sprite.getGlobalBounds();
}

bool Bullet::isOutOfBounds(float screenWidth, float screenHeight) const {
    sf::Vector2f pos = sprite.getPosition();
    return (pos.x < -50 || pos.x > screenWidth + 50 || 
            pos.y < -50 || pos.y > screenHeight + 50);
}
