#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <string>

class Player {
private:
    sf::Sprite  sprite;
    sf::Texture texture;

    float speed;
    int   health;
    int   maxHealth;
    float fireRate;
    float fireTimer;
    float thrusterTimer;

    void createFallbackTexture();

public:
    Player();

    void move(float offsetX, float offsetY);
    void update(float deltaTime);

    bool  canShoot();
    void  resetFireTimer();

    void  takeDamage(int damage);
    void  heal(int amount);
    void  setMaxHealth(int maxHealth);
    int   getHealth() const;
    int   getMaxHealth() const;
    bool  isAlive() const;

    void  setFireRate(float rate);
    float getFireRate() const;

    sf::Vector2f getPosition() const;
    void         setPosition(float x, float y);

    void render(sf::RenderWindow& window);

    void setTexture(const std::string& texturePath);
    bool loadTexture(const std::string& texturePath);

    sf::FloatRect getBounds() const;
    sf::Sprite    getSprite() const;

    void promptUserUpload();
};

#endif // PLAYER_H
