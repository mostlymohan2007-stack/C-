#include "Player.h"
#include <iostream>

Player::Player() : speed(300.0f), health(100), maxHealth(100), fireRate(0.2f), fireTimer(0.0f) {
    // Load player texture
    if (!texture.loadFromFile("assets/player.png")) {
        // Create a simple colored rectangle as fallback if texture fails to load
        texture.create(40, 40);
        sf::Image image = texture.copyToImage();
        image.create(40, 40, sf::Color::Blue);
        texture.update(image);
    }
    
    sprite.setTexture(texture);
    sprite.setPosition(400.0f, 500.0f); // Start at bottom center
    sprite.setOrigin(sprite.getLocalBounds().width / 2.0f, sprite.getLocalBounds().height / 2.0f);
}

void Player::move(float offsetX, float offsetY) {
    sprite.move(offsetX, offsetY);
}

void Player::update(float deltaTime) {
    // Update fire timer
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
