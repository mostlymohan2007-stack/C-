#ifndef BULLET_H
#define BULLET_H

#include <SFML/Graphics.hpp>

enum class BulletType {
    PLAYER,
    ENEMY
};

class Bullet {
private:
    sf::Sprite   sprite;
    sf::Texture  texture;
    BulletType   type;
    bool         active;
    int          damage;
    float        speed;
    sf::Vector2f direction;
    float        animTimer;

    void createBulletTexture();

public:
    Bullet(BulletType type, float x, float y, float dirX, float dirY);

    void update(float deltaTime);

    bool       isActive()  const;
    void       setActive(bool active);
    BulletType getType()   const;
    int        getDamage() const;

    sf::Vector2f  getPosition()          const;
    void          setPosition(float x, float y);

    void render(sf::RenderWindow& window);

    sf::FloatRect getBounds() const;
    bool isOutOfBounds(float screenWidth, float screenHeight) const;
};

#endif // BULLET_H
