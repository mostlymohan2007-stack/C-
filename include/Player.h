#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>

class Player {
private:
    sf::Sprite sprite;
    sf::Texture texture;
    float speed;
    int health;
    int maxHealth;
    float fireRate;
    float fireTimer;
    
    void loadUserTextures();
    void createFallbackTexture();
    
public:
    Player();
    
    // Movement methods
    void move(float offsetX, float offsetY);
    void update(float deltaTime);
    
    // Combat methods
    bool canShoot();
    void resetFireTimer();
    
    // Health methods
    void takeDamage(int damage);
    void heal(int amount);
    int getHealth() const;
    int getMaxHealth() const;
    bool isAlive() const;
    
    // Position methods
    sf::Vector2f getPosition() const;
    void setPosition(float x, float y);
    
    // Rendering
    void render(sf::RenderWindow& window);
    
    // Bounds for collision detection
    sf::FloatRect getBounds() const;
    
    // Get sprite for collision detection
    sf::Sprite getSprite() const;
    
    // User upload functionality
    void promptUserUpload();
};

#endif // PLAYER_H
