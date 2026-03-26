#include "Bullet.h"
#include <iostream>
#include <cmath>

Bullet::Bullet(BulletType type, float x, float y, float dirX, float dirY)
    : type(type), active(true), animTimer(0.0f)
{
    switch (type) {
        case BulletType::PLAYER:
            speed = 520.0f;
            damage = 10;
            break;
        case BulletType::ENEMY:
            speed = 310.0f;
            damage = 15;
            break;
    }

    direction.x = dirX;
    direction.y = dirY;

    createBulletTexture();
    sprite.setTexture(texture);
    sprite.setPosition(x, y);
    sprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);
}

void Bullet::createBulletTexture() {
    if (type == BulletType::PLAYER) {
        // Cyan laser bolt
        const int W = 6, H = 18;
        sf::Image img;
        img.create(W, H, sf::Color::Transparent);
        // Bright core
        for (int y = 2; y < H - 2; ++y) {
            img.setPixel(2, y, sf::Color(200, 240, 255));
            img.setPixel(3, y, sf::Color(200, 240, 255));
        }
        // Glow edges
        for (int y = 0; y < H; ++y) {
            float alpha = 180.0f * (1.0f - std::abs(y - H/2.0f) / (H/2.0f));
            if (y >= 0 && y < H) {
                img.setPixel(1, y, sf::Color(100, 200, 255, static_cast<sf::Uint8>(alpha)));
                img.setPixel(4, y, sf::Color(100, 200, 255, static_cast<sf::Uint8>(alpha)));
            }
        }
        // Tip glow
        img.setPixel(2, 0, sf::Color(255, 255, 255));
        img.setPixel(3, 0, sf::Color(255, 255, 255));
        texture.create(W, H);
        texture.update(img);
    } else {
        // Orange-red plasma ball
        const int W = 10, H = 10;
        sf::Image img;
        img.create(W, H, sf::Color::Transparent);
        for (int y = 0; y < H; ++y) {
            for (int x = 0; x < W; ++x) {
                float dx = x - W/2.0f + 0.5f;
                float dy = y - H/2.0f + 0.5f;
                float dist = std::sqrt(dx*dx + dy*dy);
                if (dist < W/2.0f) {
                    float t = 1.0f - dist / (W/2.0f);
                    img.setPixel(x, y, sf::Color(
                        255,
                        static_cast<sf::Uint8>(100 * t),
                        50,
                        static_cast<sf::Uint8>(230 * t)
                    ));
                }
            }
        }
        texture.create(W, H);
        texture.update(img);
    }
}

void Bullet::update(float deltaTime) {
    animTimer += deltaTime;
    sprite.move(direction.x * speed * deltaTime,
                direction.y * speed * deltaTime);

    // Pulsate alpha for enemy bullets
    if (type == BulletType::ENEMY) {
        float pulse = 0.8f + 0.2f * std::sin(animTimer * 15.0f);
        sprite.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(200 * pulse)));
    }
}

bool       Bullet::isActive()   const { return active; }
void       Bullet::setActive(bool a)  { active = a; }
BulletType Bullet::getType()    const { return type; }
int        Bullet::getDamage()  const { return damage; }

sf::Vector2f Bullet::getPosition() const { return sprite.getPosition(); }
void         Bullet::setPosition(float x, float y) { sprite.setPosition(x, y); }

void Bullet::render(sf::RenderWindow& window) {
    if (!active) return;

    // Draw glow halo first
    sf::CircleShape glow;
    if (type == BulletType::PLAYER) {
        float glowR = 6.0f + 2.0f * std::abs(std::sin(animTimer * 10.0f));
        glow.setRadius(glowR);
        glow.setOrigin(glowR, glowR);
        glow.setPosition(sprite.getPosition());
        glow.setFillColor(sf::Color(80, 180, 255, 50));
        window.draw(glow);
    } else {
        float glowR = 8.0f;
        glow.setRadius(glowR);
        glow.setOrigin(glowR, glowR);
        glow.setPosition(sprite.getPosition());
        glow.setFillColor(sf::Color(255, 80, 0, 60));
        window.draw(glow);
    }

    window.draw(sprite);
}

sf::FloatRect Bullet::getBounds() const { return sprite.getGlobalBounds(); }

bool Bullet::isOutOfBounds(float screenW, float screenH) const {
    sf::Vector2f pos = sprite.getPosition();
    return (pos.x < -30 || pos.x > screenW + 30 ||
            pos.y < -30 || pos.y > screenH + 30);
}
