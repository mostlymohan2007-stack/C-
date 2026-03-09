#ifndef ENEMY_H
#define ENEMY_H

#include <SFML/Graphics.hpp>

enum class EnemyType {
    BASIC,
    FAST,
    HEAVY
};

class Enemy {
private:
    sf::Sprite sprite;
    sf::Texture texture;
    EnemyType type;
    float speed;
    int health;
    int maxHealth;
    float fireRate;
    float fireTimer;
    float shootChance;
    sf::Vector2f direction;
    
    void loadUserEnemyTextures();
    void createFallbackEnemyTexture();
    
public:
    Enemy(EnemyType type = EnemyType::BASIC, float x = 0.0f, float y = 0.0f);
    
    // Movement methods
    void update(float deltaTime);
    void setDirection(float x, float y);
    
    // Combat methods
    bool canShoot();
    bool shouldShoot();
    void resetFireTimer();
    void takeDamage(int damage);
    
    // Status methods
    bool isAlive() const;
    EnemyType getType() const;
    int getHealth() const;
    
    // Position methods
    sf::Vector2f getPosition() const;
    void setPosition(float x, float y);
    
    // Rendering
    void render(sf::RenderWindow& window);
    
    // Bounds for collision detection
    sf::FloatRect getBounds() const;
    
    // Get sprite for collision detection
    sf::Sprite getSprite() const;
    
    // Score value when destroyed
    int getScoreValue() const;
    
    // User upload functionality
    void promptUserUpload();
};

#endif // ENEMY_H
