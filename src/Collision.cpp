#include "Collision.h"

bool Collision::checkAABB(const sf::FloatRect& rect1, const sf::FloatRect& rect2) {
    return rect1.intersects(rect2);
}

bool Collision::checkSpriteCollision(const sf::Sprite& sprite1, const sf::Sprite& sprite2) {
    return checkAABB(sprite1.getGlobalBounds(), sprite2.getGlobalBounds());
}

bool Collision::checkBulletSpriteCollision(const Bullet& bullet, const sf::Sprite& sprite) {
    if (!bullet.isActive()) return false;
    return checkAABB(bullet.getBounds(), sprite.getGlobalBounds());
}

sf::Vector2f Collision::getCollisionResponse(const sf::FloatRect& rect1, const sf::FloatRect& rect2) {
    sf::Vector2f response(0.0f, 0.0f);
    
    // Calculate overlap on each axis
    float overlapX = std::min(rect1.left + rect1.width, rect2.left + rect2.width) - 
                     std::max(rect1.left, rect2.left);
    float overlapY = std::min(rect1.top + rect1.height, rect2.top + rect2.height) - 
                     std::max(rect1.top, rect2.top);
    
    // Determine the smallest overlap and push in that direction
    if (overlapX < overlapY) {
        // Horizontal collision
        if (rect1.left < rect2.left) {
            response.x = -overlapX; // Push left
        } else {
            response.x = overlapX;  // Push right
        }
    } else {
        // Vertical collision
        if (rect1.top < rect2.top) {
            response.y = -overlapY; // Push up
        } else {
            response.y = overlapY;  // Push down
        }
    }
    
    return response;
}

bool Collision::isWithinBounds(const sf::FloatRect& rect, float screenWidth, float screenHeight) {
    return (rect.left >= 0 && 
            rect.left + rect.width <= screenWidth &&
            rect.top >= 0 && 
            rect.top + rect.height <= screenHeight);
}
