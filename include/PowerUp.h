#ifndef POWERUP_H
#define POWERUP_H

#include <SFML/Graphics.hpp>
#include <random>
#include <string>

enum class PowerUpType {
    RAPID_FIRE,    // ✨ Triple shot for 5 seconds
    HEALTH,        // 🧚 20% max health restore
    COUNT
};

class PowerUp {
private:
    sf::Sprite   sprite;
    sf::Texture  texture;
    PowerUpType  type;
    sf::Vector2f position;
    float        speed;
    bool         active;
    float        lifetime;
    float        animTimer;

    void createFallbackTexture();

public:
    PowerUp(float x = 0.0f, float y = 0.0f);

    void update(float deltaTime);

    bool isActive()  const;
    void setActive(bool active);
    bool isExpired() const;

    sf::Vector2f getPosition()          const;
    void         setPosition(float x, float y);

    PowerUpType  getType()        const;
    std::string  getTypeName()    const;
    std::string  getDescription() const;

    void render(sf::RenderWindow& window);

    sf::FloatRect getBounds() const;
    bool isOutOfBounds(float screenWidth, float screenHeight) const;

    float applyEffect();
    sf::Sprite getSprite() const;
};

#endif // POWERUP_H
