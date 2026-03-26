#include "PowerUp.h"
#include <iostream>
#include <cmath>

PowerUp::PowerUp(float x, float y)
    : position(x, y), speed(55.0f), active(true), lifetime(6.0f), animTimer(0.0f)
{
    std::random_device rd;
    std::uniform_int_distribution<int> dist(0, static_cast<int>(PowerUpType::COUNT) - 1);
    type = static_cast<PowerUpType>(dist(rd));

    // Try to load PNG asset for HEALTH power-up
    bool pngLoaded = false;
    if (type == PowerUpType::HEALTH) {
        sf::Image img;
        if (img.loadFromFile("assets/powerup_health.png")) {
            // Remove very dark near-black background pixels if any
            sf::Vector2u sz = img.getSize();
            for (unsigned py = 0; py < sz.y; ++py)
                for (unsigned px2 = 0; px2 < sz.x; ++px2) {
                    sf::Color c = img.getPixel(px2, py);
                    if (c.r <= 25 && c.g <= 25 && c.b <= 25 && c.a >= 250)
                        img.setPixel(px2, py, sf::Color::Transparent);
                }
            texture.loadFromImage(img);
            texture.setSmooth(true);
            // Scale to 16 px (made MUCH SMALLER, was 24px)
            float sc = 16.0f / std::max((float)texture.getSize().x,
                                        (float)texture.getSize().y);
            sprite.setTexture(texture, true);
            sprite.setScale(sc, sc);
            sprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);
            pngLoaded = true;
            std::cout << "PowerUp texture loaded: assets/powerup_health.png" << std::endl;
        }
    }

    if (!pngLoaded) {
        createFallbackTexture();
        sprite.setTexture(texture);
        sprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);
    }

    sprite.setPosition(position);
}

void PowerUp::createFallbackTexture() {
    const int W = 28, H = 28;
    sf::Image img;
    img.create(W, H, sf::Color::Transparent);

    if (type == PowerUpType::RAPID_FIRE) {
        // ✨ Star / lightning bolt shape - Gold
        // Draw a star
        for (int y = 0; y < H; ++y) {
            for (int x = 0; x < W; ++x) {
                float dx = x - W/2.0f + 0.5f;
                float dy = y - H/2.0f + 0.5f;
                float dist = std::sqrt(dx*dx + dy*dy);
                // 5-pointed star approximation
                float angle = std::atan2(dy, dx);
                float star = std::cos(5.0f * angle) * 0.3f + 0.7f;
                if (dist < (W/2.0f) * star) {
                    float t = 1.0f - dist / ((W/2.0f) * star);
                    img.setPixel(x, y, sf::Color(
                        255,
                        static_cast<sf::Uint8>(200 + 55 * t),
                        static_cast<sf::Uint8>(50 * t),
                        static_cast<sf::Uint8>(230)
                    ));
                }
            }
        }
        // Center bright spot
        img.setPixel(13, 13, sf::Color(255, 255, 200));
        img.setPixel(14, 13, sf::Color(255, 255, 200));
        img.setPixel(13, 14, sf::Color(255, 255, 200));
        img.setPixel(14, 14, sf::Color(255, 255, 200));
    } else {
        // 🧚 Health - Green heart / cross
        // Draw a cross
        for (int y = 8; y < 20; ++y)
            for (int x = 4; x < 24; ++x)
                img.setPixel(x, y, sf::Color(50, 220, 100));
        for (int y = 4; y < 24; ++y)
            for (int x = 8; x < 20; ++x)
                img.setPixel(x, y, sf::Color(50, 220, 100));
        // Bright center
        for (int y = 10; y < 18; ++y)
            for (int x = 10; x < 18; ++x)
                img.setPixel(x, y, sf::Color(150, 255, 180));
        // White highlight
        img.setPixel(13, 13, sf::Color(255, 255, 255));
        img.setPixel(14, 13, sf::Color(255, 255, 255));
    }

    texture.create(W, H);
    texture.update(img);
}

void PowerUp::update(float deltaTime) {
    if (!active) return;
    animTimer += deltaTime;

    // Float downward + gentle sine bob
    float bob = std::sin(animTimer * 3.0f) * 0.5f;
    position.y += (speed + bob) * deltaTime;
    sprite.setPosition(position);

    // Gentle rotation
    sprite.setRotation(animTimer * 45.0f);

    // Pulsing scale
    float scale = 1.0f + 0.15f * std::sin(animTimer * 4.0f);
    sprite.setScale(scale, scale);

    // Fade out near end of lifetime
    lifetime -= deltaTime;
    if (lifetime < 1.5f) {
        float alpha = lifetime / 1.5f;
        sprite.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(255 * alpha)));
    }

    if (lifetime <= 0) active = false;
}

bool       PowerUp::isActive()   const { return active; }
void       PowerUp::setActive(bool a)  { active = a; }
bool       PowerUp::isExpired()  const { return lifetime <= 0; }

sf::Vector2f PowerUp::getPosition() const { return position; }
void PowerUp::setPosition(float x, float y) {
    position = {x, y};
    sprite.setPosition(position);
}

PowerUpType  PowerUp::getType()    const { return type; }

std::string PowerUp::getTypeName() const {
    switch (type) {
        case PowerUpType::RAPID_FIRE: return "Rapid Fire (5s)";
        case PowerUpType::HEALTH:     return "Health Boost (+20%)";
        default: return "Unknown";
    }
}

std::string PowerUp::getDescription() const {
    switch (type) {
        case PowerUpType::RAPID_FIRE: return "Triple shot for 5 seconds!";
        case PowerUpType::HEALTH:     return "Restores 20% of max HP";
        default: return "";
    }
}

void PowerUp::render(sf::RenderWindow& window) {
    if (!active) return;

    // Outer glow ring
    sf::CircleShape ring;
    float glowR = 18.0f + 4.0f * std::sin(animTimer * 5.0f);
    ring.setRadius(glowR);
    ring.setOrigin(glowR, glowR);
    ring.setPosition(position);
    sf::Color glowColor = (type == PowerUpType::RAPID_FIRE)
        ? sf::Color(255, 220, 50, 50)
        : sf::Color(50, 255, 120, 50);
    ring.setFillColor(sf::Color::Transparent);
    ring.setOutlineColor(glowColor);
    ring.setOutlineThickness(2.5f);
    window.draw(ring);

    // Inner glow
    sf::CircleShape inner(12.0f);
    inner.setOrigin(12, 12);
    inner.setPosition(position);
    sf::Color innerColor = (type == PowerUpType::RAPID_FIRE)
        ? sf::Color(255, 200, 30, 30) : sf::Color(30, 255, 100, 30);
    inner.setFillColor(innerColor);
    window.draw(inner);

    window.draw(sprite);
}

sf::FloatRect PowerUp::getBounds() const { return sprite.getGlobalBounds(); }
sf::Sprite    PowerUp::getSprite() const { return sprite; }

bool PowerUp::isOutOfBounds(float, float screenH) const {
    return position.y > screenH + 50.0f;
}

float PowerUp::applyEffect() {
    switch (type) {
        case PowerUpType::RAPID_FIRE: return 5.0f;
        case PowerUpType::HEALTH:     return 0.0f;
        default: return 0.0f;
    }
}
