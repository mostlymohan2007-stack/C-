#ifndef COLLISION_H
#define COLLISION_H

#include <SFML/Graphics.hpp>
#include "Bullet.h"

// Collision detection utility class
class Collision {
public:
    // Axis-Aligned Bounding Box (AABB) collision detection
    static bool checkAABB(const sf::FloatRect& rect1, const sf::FloatRect& rect2);
    
    // Check collision between two sprites
    static bool checkSpriteCollision(const sf::Sprite& sprite1, const sf::Sprite& sprite2);
    
    // Check collision between bullet and sprite
    static bool checkBulletSpriteCollision(const Bullet& bullet, const sf::Sprite& sprite);
    
    // Get collision response (optional for more advanced physics)
    static sf::Vector2f getCollisionResponse(const sf::FloatRect& rect1, const sf::FloatRect& rect2);
    
    // Check if object is within screen bounds
    static bool isWithinBounds(const sf::FloatRect& rect, float screenWidth, float screenHeight);
};

#endif // COLLISION_H
