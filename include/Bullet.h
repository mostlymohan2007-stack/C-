#ifndef BULLET_H
#define BULLET_H

#include <SFML/Graphics.hpp>

enum class BulletType {
    PLAYER,
    ENEMY
};

class Bullet {
private:
    sf::Sprite sprite;
    sf::Texture texture;
    BulletType type;
    float speed;
    sf::Vector2f direction;
    bool active;
    int damage;
    
    void loadUserBulletTextures();
    void createFallbackBulletTexture();
    
public:
    Bullet(BulletType type = BulletType::PLAYER, float x = 0.0f, float y = 0.0f, 
           float dirX = 0.0f, float dirY = -1.0f);
    
    // Movement methods
    void update(float deltaTime);
    
    // Status methods
    bool isActive() const;
    void setActive(bool active);
    BulletType getType() const;
    int getDamage() const;
    
    // Position methods
    sf::Vector2f getPosition() const;
    void setPosition(float x, float y);
    
    // Rendering
    void render(sf::RenderWindow& window);
    
    // Bounds for collision detection
    sf::FloatRect getBounds() const;
    
    // Check if bullet is out of screen bounds
    bool isOutOfBounds(float screenWidth, float screenHeight) const;
    
    // User upload functionality
    void promptUserUpload();
};

#endif // BULLET_H
