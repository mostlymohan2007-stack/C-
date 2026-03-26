#include "Player.h"
#include <iostream>
#include <cmath>

Player::Player()
    : speed(240.0f), health(100), maxHealth(100), fireRate(0.18f), fireTimer(0.0f),
      thrusterTimer(0.0f)
{
    createFallbackTexture();
    sprite.setTexture(texture);
    sprite.setPosition(450.0f, 580.0f);
    sprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);
    std::cout << "Player initialized (rocket ship)" << std::endl;
}

void Player::move(float offsetX, float offsetY) {
    sprite.move(offsetX, offsetY);
    
    // Simple rotation: only rotate when actively moving
    if (offsetX > 0.1f) {
        sprite.setRotation(6.0f);   // bank right
    } else if (offsetX < -0.1f) {
        sprite.setRotation(-6.0f);  // bank left
    } else {
        // Gradually return to center when not turning
        float currentRotation = sprite.getRotation();
        if (currentRotation > 0) {
            sprite.setRotation(currentRotation - 2.0f);
        } else if (currentRotation < 0) {
            sprite.setRotation(currentRotation + 2.0f);
        }
    }
}

void Player::update(float deltaTime) {
    if (fireTimer > 0.0f) {
        fireTimer -= deltaTime;
    }
    
    // Simple thruster animation
    thrusterTimer += deltaTime;
}

bool Player::canShoot() { return fireTimer <= 0.0f; }
void Player::resetFireTimer() { fireTimer = fireRate; }

void Player::takeDamage(int damage) {
    health -= damage;
    if (health < 0) health = 0;
}

void Player::heal(int amount) {
    health += amount;
    if (health > maxHealth) health = maxHealth;
}

int  Player::getHealth() const    { return health; }
int  Player::getMaxHealth() const { return maxHealth; }

void Player::setMaxHealth(int mh) {
    maxHealth = mh;
    if (health > maxHealth) health = maxHealth;
}

bool Player::isAlive() const { return health > 0; }

void Player::setFireRate(float rate) { fireRate = rate; }
float Player::getFireRate() const    { return fireRate; }

sf::Vector2f Player::getPosition() const { return sprite.getPosition(); }
void Player::setPosition(float x, float y) { sprite.setPosition(x, y); }

void Player::render(sf::RenderWindow& window) {
    // Thruster glow below ship
    float thrusterAlpha = 160.0f + 95.0f * std::abs(std::sin(thrusterTimer * 12.0f));
    sf::RectangleShape thruster(sf::Vector2f(10, 18));
    thruster.setOrigin(5, 0);
    sf::Vector2f pos = sprite.getPosition();
    // Place thruster from underneath the sprite
    sf::FloatRect bounds = sprite.getGlobalBounds();
    thruster.setPosition(pos.x, bounds.top + bounds.height - 4);
    thruster.setFillColor(sf::Color(80, 160, 255, static_cast<sf::Uint8>(thrusterAlpha)));
    window.draw(thruster);

    // Secondary smaller thrusters
    sf::RectangleShape thrL(sf::Vector2f(6, 12));
    thrL.setOrigin(3, 0);
    thrL.setPosition(pos.x - 14, bounds.top + bounds.height - 2);
    thrL.setFillColor(sf::Color(120, 200, 255, static_cast<sf::Uint8>(thrusterAlpha * 0.6f)));
    window.draw(thrL);

    sf::RectangleShape thrR(sf::Vector2f(6, 12));
    thrR.setOrigin(3, 0);
    thrR.setPosition(pos.x + 14, bounds.top + bounds.height - 2);
    thrR.setFillColor(sf::Color(120, 200, 255, static_cast<sf::Uint8>(thrusterAlpha * 0.6f)));
    window.draw(thrR);

    window.draw(sprite);
}

sf::FloatRect Player::getBounds()  const { return sprite.getGlobalBounds(); }
sf::Sprite    Player::getSprite()  const { return sprite; }

void Player::setTexture(const std::string& texturePath) {
    loadTexture(texturePath);
}

bool Player::loadTexture(const std::string& texturePath) {
    sf::Image img;
    if (img.loadFromFile(texturePath)) {
        // PNG assets carry proper alpha – only scrub near-white JPG residue
        sf::Vector2u sz = img.getSize();
        for (unsigned y = 0; y < sz.y; ++y) {
            for (unsigned x = 0; x < sz.x; ++x) {
                sf::Color c = img.getPixel(x, y);
                if (c.r >= 220 && c.g >= 220 && c.b >= 220)
                    img.setPixel(x, y, sf::Color::Transparent);
            }
        }
        texture.loadFromImage(img);
        texture.setSmooth(true);
        sprite.setTexture(texture, true);
        // Scale to 72x72 for a bigger, clearer ship sprite
        float targetSize = 72.0f;
        float scaleVal = targetSize / std::max((float)texture.getSize().x,
                                               (float)texture.getSize().y);
        sprite.setScale(scaleVal, scaleVal);
        sprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);
        std::cout << "Player texture loaded (PNG alpha): " << texturePath << std::endl;
        return true;
    }
    std::cout << "Player texture failed: " << texturePath << " - using fallback" << std::endl;
    createFallbackTexture();
    return false;
}

void Player::createFallbackTexture() {
    // Draw a nice rocket ship pixel-art style
    const int W = 40, H = 56;
    sf::Image img;
    img.create(W, H, sf::Color::Transparent);

    // Body (blue-cyan gradient)
    for (int y = 8; y < 46; ++y) {
        for (int x = 10; x < 30; ++x) {
            float t = static_cast<float>(y - 8) / 38.0f;
            sf::Uint8 r = static_cast<sf::Uint8>(40 + 20 * t);
            sf::Uint8 g = static_cast<sf::Uint8>(120 + 60 * (1-t));
            sf::Uint8 b = static_cast<sf::Uint8>(220 + 35 * (1-t));
            img.setPixel(x, y, sf::Color(r, g, b));
        }
    }
    // Nose cone (triangle, top)
    for (int y = 0; y < 10; ++y) {
        int dx = 9 - y;
        for (int x = 10 + dx; x < 30 - dx; ++x)
            img.setPixel(x, y, sf::Color(80, 200, 255));
    }
    // Wings
    for (int y = 30; y < 48; ++y) {
        int wings = (y - 30) / 3;
        for (int x = 10 - wings; x < 10; ++x)
            if (x >= 0) img.setPixel(x, y, sf::Color(30, 80, 180));
        for (int x = 30; x < 30 + wings; ++x)
            if (x < W)  img.setPixel(x, y, sf::Color(30, 80, 180));
    }
    // Cockpit window
    for (int y = 12; y < 22; ++y)
        for (int x = 16; x < 24; ++x)
            img.setPixel(x, y, sf::Color(180, 240, 255, 200));
    // Engine glow bottom
    for (int y = 44; y < H; ++y)
        for (int x = 14; x < 26; ++x)
            img.setPixel(x, y, sf::Color(100, 180, 255, static_cast<sf::Uint8>(220 - (y-44)*30)));

    texture.create(W, H);
    texture.update(img);
    sprite.setTexture(texture, true);
    sprite.setOrigin(W / 2.0f, H / 2.0f);
}

void Player::promptUserUpload() {
    std::cout << "Place 'player_ship.png' in assets/ folder to use custom ship." << std::endl;
}
