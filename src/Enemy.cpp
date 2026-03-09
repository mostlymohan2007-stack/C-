#include "Enemy.h"
#include <cstdlib>
#include <ctime>

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
    
    // Load enemy texture
    if (!texture.loadFromFile("assets/enemy.png")) {
        // Create a simple colored rectangle as fallback
        texture.create(30, 30);
        sf::Image image = texture.copyToImage();
        switch (type) {
            case EnemyType::BASIC:
                image.create(30, 30, sf::Color::Red);
                break;
            case EnemyType::FAST:
                image.create(25, 25, sf::Color::Yellow);
                break;
            case EnemyType::HEAVY:
                image.create(40, 40, sf::Color::Magenta);
                break;
        }
        texture.update(image);
    }
    
    sprite.setTexture(texture);
    sprite.setPosition(x, y);
    sprite.setOrigin(sprite.getLocalBounds().width / 2.0f, sprite.getLocalBounds().height / 2.0f);
    
    // Set initial movement direction (downward with slight random horizontal movement)
    direction.x = (rand() % 100 - 50) / 100.0f; // Random between -0.5 and 0.5
    direction.y = 1.0f; // Move downward
}

void Enemy::update(float deltaTime) {
    // Move the enemy
    sprite.move(direction.x * speed * deltaTime, direction.y * speed * deltaTime);
    
    // Update fire timer
    if (fireTimer > 0.0f) {
        fireTimer -= deltaTime;
    }
}

void Enemy::setDirection(float x, float y) {
    direction.x = x;
    direction.y = y;
}

bool Enemy::canShoot() {
    return fireTimer <= 0.0f;
}

bool Enemy::shouldShoot() {
    return canShoot() && (rand() % 1000 < shootChance * 1000);
}

void Enemy::resetFireTimer() {
    fireTimer = fireRate;
}

void Enemy::takeDamage(int damage) {
    health -= damage;
    if (health < 0) health = 0;
}

bool Enemy::isAlive() const {
    return health > 0;
}

EnemyType Enemy::getType() const {
    return type;
}

int Enemy::getHealth() const {
    return health;
}

sf::Vector2f Enemy::getPosition() const {
    return sprite.getPosition();
}

void Enemy::setPosition(float x, float y) {
    sprite.setPosition(x, y);
}

void Enemy::render(sf::RenderWindow& window) {
    if (isAlive()) {
        window.draw(sprite);
    }
}

sf::FloatRect Enemy::getBounds() const {
    return sprite.getGlobalBounds();
}

sf::Sprite Enemy::getSprite() const {
    return sprite;
}

int Enemy::getScoreValue() const {
    switch (type) {
        case EnemyType::BASIC: return 10;
        case EnemyType::FAST: return 20;
        case EnemyType::HEAVY: return 30;
        default: return 10;
    }
}
