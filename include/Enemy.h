#ifndef ENEMY_H
#define ENEMY_H

#include <SFML/Graphics.hpp>
#include <string>

enum class EnemyType {
    BASIC,   // 👽 Alien
    FAST,    // 🤖 Robot drone
    HEAVY    // 👹 Demon overlord
};

class Enemy {
private:
    sf::Sprite  sprite;
    sf::Texture texture;

    EnemyType   type;
    float       speed;
    int         health;
    int         maxHealth;
    float       fireRate;
    float       fireTimer;
    float       shootChance;
    sf::Vector2f direction;

    float  animTimer;
    float  wobblePhase;
    float  flashTimer;
    float  baseScaleX;
    float  baseScaleY;
    int    scoreValue;
    bool   hasRealTexture;  // true when a PNG asset was loaded

    void createFallbackEnemyTexture();

public:
    Enemy(EnemyType type = EnemyType::BASIC, float x = 0.0f, float y = 0.0f);

    // SFML sprite holds a raw ptr to texture -- must re-bind after any copy
    Enemy(const Enemy& other);
    Enemy& operator=(const Enemy& other);

    void update(float deltaTime);
    void setDirection(float x, float y);

    bool canShoot();
    bool shouldShoot();
    void resetFireTimer();
    void takeDamage(int damage);

    bool      isAlive()     const;
    EnemyType getType()     const;
    int       getHealth()   const;
    int       getScoreValue() const;

    sf::Vector2f getPosition()         const;
    void         setPosition(float x, float y);

    void render(sf::RenderWindow& window);

    sf::FloatRect getBounds() const;
    sf::Sprite    getSprite() const;

    void  setTexture(const std::string& texturePath);
    bool  loadTexture(const std::string& texturePath);

    void  setSpeed(float speed);
    float getSpeed() const;

    void promptUserUpload();
};

#endif // ENEMY_H
