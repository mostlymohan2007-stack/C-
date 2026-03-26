#include "Enemy.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <algorithm>

Enemy::Enemy(EnemyType type, float x, float y)
    : type(type), fireTimer(0.0f), animTimer(0.0f), wobblePhase(0.0f),
      flashTimer(0.0f), baseScaleX(1.0f), baseScaleY(1.0f), hasRealTexture(false)
{
    switch (type) {
        case EnemyType::BASIC:
            speed = 90.0f;
            health = 30; maxHealth = 30;
            fireRate = 2.5f; shootChance = 0.008f;
            scoreValue = 10;
            break;
        case EnemyType::FAST:
            speed = 210.0f;
            health = 20; maxHealth = 20;
            fireRate = 1.8f; shootChance = 0.014f;
            scoreValue = 20;
            break;
        case EnemyType::HEAVY:
            speed = 55.0f;
            health = 80; maxHealth = 80;
            fireRate = 3.5f; shootChance = 0.012f;
            scoreValue = 35;
            break;
    }

    createFallbackEnemyTexture();
    sprite.setTexture(texture);
    sprite.setPosition(x, y);
    sprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);

    direction.x = (rand() % 100 - 50) / 200.0f;
    direction.y = 1.0f;
    wobblePhase = static_cast<float>(rand() % 628) / 100.0f;
}

// ---------------------------------------------------------------
// Copy constructor -- REQUIRED because sf::Sprite stores a raw
// pointer to sf::Texture.  A default copy would leave the sprite
// pointing at the SOURCE enemy's texture (dangling after that
// enemy is destroyed), producing a white/invalid rectangle.
// ---------------------------------------------------------------
Enemy::Enemy(const Enemy& o)
    : type(o.type), speed(o.speed),
      health(o.health), maxHealth(o.maxHealth),
      fireRate(o.fireRate), fireTimer(o.fireTimer),
      shootChance(o.shootChance), direction(o.direction),
      animTimer(o.animTimer), wobblePhase(o.wobblePhase),
      flashTimer(o.flashTimer),
      baseScaleX(o.baseScaleX), baseScaleY(o.baseScaleY),
      scoreValue(o.scoreValue), hasRealTexture(o.hasRealTexture)
{
    texture = o.texture;          // copy GPU texture data
    sprite  = o.sprite;           // copy transform / scale / colour
    sprite.setTexture(texture);   // *** re-bind to OUR texture ***
}

Enemy& Enemy::operator=(const Enemy& o) {
    if (this == &o) return *this;
    type          = o.type;
    speed         = o.speed;
    health        = o.health;       maxHealth  = o.maxHealth;
    fireRate      = o.fireRate;     fireTimer  = o.fireTimer;
    shootChance   = o.shootChance;  direction  = o.direction;
    animTimer     = o.animTimer;    wobblePhase= o.wobblePhase;
    flashTimer    = o.flashTimer;
    baseScaleX    = o.baseScaleX;   baseScaleY = o.baseScaleY;
    scoreValue    = o.scoreValue;   hasRealTexture = o.hasRealTexture;
    texture = o.texture;
    sprite  = o.sprite;
    sprite.setTexture(texture);     // *** re-bind to OUR texture ***
    return *this;
}

void Enemy::update(float deltaTime) {
    animTimer   += deltaTime;
    wobblePhase += deltaTime;

    float wobble = std::sin(wobblePhase * 1.8f) * 0.15f;
    sprite.move((direction.x + wobble) * speed * deltaTime,
                 direction.y            * speed * deltaTime);

    // Wrap horizontally
    float px = sprite.getPosition().x;
    if      (px < -30) sprite.setPosition(930.0f,  sprite.getPosition().y);
    else if (px > 930) sprite.setPosition(-30.0f, sprite.getPosition().y);

    if (fireTimer > 0.0f) fireTimer -= deltaTime;

    // HEAVY: pulsating scale
    if (type == EnemyType::HEAVY) {
        float sc = 1.0f + 0.06f * std::sin(animTimer * 3.0f);
        sprite.setScale(sc * baseScaleX, sc * baseScaleY);
    }
    // FAST: spin only for code-drawn sprite; tilt gently when using real PNG
    if (type == EnemyType::FAST) {
        if (!hasRealTexture) {
            sprite.rotate(60.0f * deltaTime);          // full spin for fallback art
        } else {
            // Gentle oscillating roll for PNG robot — looks intentional, not broken
            float tilt = 12.0f * std::sin(animTimer * 4.0f);
            sprite.setRotation(tilt);
        }
    }
    // Flash reset
    if (flashTimer > 0.0f) {
        flashTimer -= deltaTime;
        if (flashTimer <= 0.0f) {
            sprite.setColor(sf::Color::White);
            flashTimer = 0.0f;
        }
    }
}

void Enemy::setDirection(float x, float y) { direction.x = x; direction.y = y; }
bool Enemy::canShoot()     { return fireTimer <= 0.0f; }
bool Enemy::shouldShoot()  { return canShoot() && (rand() % 1000 < (int)(shootChance * 1000)); }
void Enemy::resetFireTimer() { fireTimer = fireRate; }

void Enemy::takeDamage(int damage) {
    health -= damage;
    if (health < 0) health = 0;
    sprite.setColor(sf::Color(255, 80, 80));
    flashTimer = 0.10f;
}

bool      Enemy::isAlive()     const { return health > 0; }
EnemyType Enemy::getType()     const { return type; }
int       Enemy::getHealth()   const { return health; }
int       Enemy::getScoreValue() const { return scoreValue; }

sf::Vector2f Enemy::getPosition() const { return sprite.getPosition(); }
void Enemy::setPosition(float x, float y) { sprite.setPosition(x, y); }

void Enemy::render(sf::RenderWindow& window) {
    if (!isAlive()) return;

    // Per-enemy health bar when damaged
    if (health < maxHealth) {
        sf::FloatRect b = sprite.getGlobalBounds();
        float maxW = b.width;
        float ratio = (float)health / (float)maxHealth;
        sf::RectangleShape bg(sf::Vector2f(maxW, 5));
        bg.setPosition(b.left, b.top - 8);
        bg.setFillColor(sf::Color(60, 0, 0, 200));
        window.draw(bg);
        sf::RectangleShape fg(sf::Vector2f(maxW * ratio, 5));
        fg.setPosition(b.left, b.top - 8);
        sf::Uint8 rr = (sf::Uint8)(255 * (1.0f - ratio));
        sf::Uint8 gr = (sf::Uint8)(220 * ratio);
        fg.setFillColor(sf::Color(rr + 35, gr, 50, 220));
        window.draw(fg);
    }

    window.draw(sprite);
}

sf::FloatRect Enemy::getBounds() const { return sprite.getGlobalBounds(); }
sf::Sprite    Enemy::getSprite() const { return sprite; }

void Enemy::setTexture(const std::string& texturePath) {
    loadTexture(texturePath);
}

bool Enemy::loadTexture(const std::string& texturePath) {
    sf::Image img;
    if (img.loadFromFile(texturePath)) {
        // PNG assets already have proper alpha channel background (transparent)
        // Just remove any near-white residue for JPG fallback compatibility
        sf::Vector2u sz = img.getSize();
        for (unsigned y = 0; y < sz.y; ++y) {
            for (unsigned x = 0; x < sz.x; ++x) {
                sf::Color c = img.getPixel(x, y);
                // Remove near-white backgrounds (JPG artifacts)
                if (c.r >= 220 && c.g >= 220 && c.b >= 220)
                    img.setPixel(x, y, sf::Color::Transparent);
                // Remove near-black solid backgrounds (dark checkered bg artifacts)
                if (c.r <= 30 && c.g <= 30 && c.b <= 30 && c.a >= 250)
                    img.setPixel(x, y, sf::Color::Transparent);
            }
        }
        texture.loadFromImage(img);
        texture.setSmooth(true);
        sprite.setTexture(texture, true);
        // Larger sizes for better visibility with real PNG art
        float targetSize = (type == EnemyType::HEAVY) ? 120.0f  // Made enemy demon BIGGER (was 96px)
                         : (type == EnemyType::FAST)  ? 72.0f : 80.0f;
        float scaleVal = targetSize / std::max((float)texture.getSize().x,
                                               (float)texture.getSize().y);
        baseScaleX = scaleVal;
        baseScaleY = scaleVal;
        sprite.setScale(scaleVal, scaleVal);
        sprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);
        std::cout << "Enemy texture loaded (PNG alpha): " << texturePath << std::endl;
        hasRealTexture = true;
        return true;
    }
    std::cout << "Enemy texture failed: " << texturePath << " - using pixel-art" << std::endl;
    createFallbackEnemyTexture();
    return false;
}

void Enemy::setSpeed(float s) { speed = s; }
float Enemy::getSpeed() const { return speed; }

// --------------------------------------------------------------------------
// PIXEL-ART SPRITE DRAWING  (64x64, fully transparent background)
// --------------------------------------------------------------------------
void Enemy::createFallbackEnemyTexture() {
    const int W = 64, H = 64;
    sf::Image img;
    img.create(W, H, sf::Color::Transparent);

    // Helper lambdas
    auto setRect = [&](int x0, int y0, int x1, int y1, sf::Color c) {
        for (int y = y0; y < y1; ++y)
            for (int x = x0; x < x1; ++x)
                if (x >= 0 && x < W && y >= 0 && y < H)
                    img.setPixel(x, y, c);
    };
    auto setCircle = [&](int cx, int cy, int r, sf::Color c) {
        for (int y = cy - r; y <= cy + r; ++y)
            for (int x = cx - r; x <= cx + r; ++x) {
                int dx = x - cx, dy = y - cy;
                if (dx*dx + dy*dy <= r*r && x >= 0 && x < W && y >= 0 && y < H)
                    img.setPixel(x, y, c);
            }
    };

    if (type == EnemyType::BASIC) {
        // ===== 👽  GREEN ALIEN UFO SAUCER =====

        // Outer disc (ellipse) - wide flat body
        for (int y = 32; y < 52; ++y) {
            float fy = (y - 42.0f) / 10.0f;
            float halfW = std::sqrt(std::max(0.0f, 1.0f - fy * fy)) * 27.0f;
            int x0 = (int)(32 - halfW), x1 = (int)(32 + halfW);
            for (int x = x0; x <= x1; ++x) {
                float t = (x - x0) / std::max(1.0f, (float)(x1 - x0));
                sf::Uint8 g = (sf::Uint8)(100 + 100 * (0.5f - std::abs(t - 0.5f)) * 2.0f);
                img.setPixel(x, y, sf::Color(20, g, 40));
            }
        }
        // Bright rim stripe
        for (int y = 37; y < 40; ++y) {
            float fy = (y - 42.0f) / 10.0f;
            float halfW = std::sqrt(std::max(0.0f, 1.0f - fy * fy)) * 27.0f;
            for (int x = (int)(32 - halfW); x <= (int)(32 + halfW); ++x)
                img.setPixel(x, y, sf::Color(80, 255, 120, 200));
        }
        // Dome (bubble on top)
        setCircle(32, 28, 16, sf::Color(40, 160, 80));
        setCircle(32, 28, 14, sf::Color(60, 190, 100));
        setCircle(32, 28, 12, sf::Color(70, 220, 120));
        // Dome glass inner highlight
        setCircle(27, 23, 5, sf::Color(180, 255, 200, 170));
        setCircle(26, 22, 3, sf::Color(240, 255, 240, 220));
        // Alien pupil-eyes inside dome
        setCircle(25, 30, 4, sf::Color(220, 20, 20));
        setCircle(25, 30, 2, sf::Color(255, 100, 100));
        setCircle(39, 30, 4, sf::Color(220, 20, 20));
        setCircle(39, 30, 2, sf::Color(255, 100, 100));
        // Underside lights (5 glow dots)
        for (int i = 0; i < 5; ++i)
            setCircle(14 + i * 9, 48, 3, sf::Color(160, 255, 80, 230));
        // Bottom engine glow
        setCircle(32, 54, 5, sf::Color(80, 255, 80, 140));
        setCircle(32, 54, 2, sf::Color(200, 255, 150, 200));

    } else if (type == EnemyType::FAST) {
        // ===== 🤖  PURPLE ROBOT FIGHTER JET =====

        // Central fuselage (skinny nose-forward)
        setRect(27, 4,  37, 60, sf::Color(110, 40, 190));
        setRect(29, 2,  35, 56, sf::Color(145, 70, 220));
        // Swept back wings
        for (int i = 0; i < 22; ++i) {
            int y = 18 + i;
            int w = i * 2 + 1;
            setRect(27 - w, y, 27, y + 1, sf::Color(90, 30, 170));
            setRect(37,     y, 37 + w, y + 1, sf::Color(90, 30, 170));
        }
        // Wing neon trailing edge glow
        for (int i = 0; i < 22; ++i) {
            int y  = 18 + i;
            int wx = 27 - i * 2;
            if (wx >= 0 && wx < W) img.setPixel(wx, y, sf::Color(210, 120, 255));
            wx = 37 + i * 2;
            if (wx >= 0 && wx < W) img.setPixel(wx, y, sf::Color(210, 120, 255));
        }
        // Cockpit visor
        setRect(28, 8,  36, 20, sf::Color(200, 90, 255, 200));
        setRect(30, 10, 34, 18, sf::Color(240, 190, 255, 230));
        // Robot eye slits in cockpit
        for (int x = 29; x < 35; ++x) img.setPixel(x, 13, sf::Color(255, 0, 255));
        // Hull horizontal lines
        for (int x = 29; x < 35; ++x) {
            img.setPixel(x, 28, sf::Color(190, 130, 255, 170));
            img.setPixel(x, 36, sf::Color(190, 130, 255, 170));
            img.setPixel(x, 44, sf::Color(190, 130, 255, 170));
        }
        // Main engine blast
        setCircle(32, 60, 7, sf::Color(160, 60, 255, 160));
        setCircle(32, 60, 4, sf::Color(220, 160, 255, 210));
        setCircle(32, 60, 2, sf::Color(255, 240, 255, 240));
        // Wing-tip thrusters
        setCircle(16, 54, 4, sf::Color(130, 50, 210, 160));
        setCircle(48, 54, 4, sf::Color(130, 50, 210, 160));

    } else {
        // ===== 👹  DARK RED DEMON BATTLESHIP =====

        // Multi-layer armored hull (layered depth shading)
        setRect(8,  18, 56, 56, sf::Color(120, 15, 15));
        setRect(12, 14, 52, 54, sf::Color(145, 22, 22));
        setRect(16, 10, 48, 52, sf::Color(170, 30, 30));
        setRect(20, 8,  44, 50, sf::Color(195, 40, 40));
        // Center spine glow
        setRect(28, 10, 36, 50, sf::Color(215, 55, 45));

        // Horns (4 total - outer pair + inner pair)
        for (int i = 0; i < 12; ++i) {
            int yy = 9 - i / 2;
            if (yy >= 0) {
                img.setPixel(12 + i,     yy, sf::Color(210, 70, 55));
                img.setPixel(51 - i,     yy, sf::Color(210, 70, 55));
            }
        }
        for (int i = 0; i < 8; ++i) {
            int yy = 4 - i / 3;
            if (yy >= 0) {
                img.setPixel(22 + i,     yy, sf::Color(235, 95, 70));
                img.setPixel(41 - i,     yy, sf::Color(235, 95, 70));
            }
        }

        // Three-layer glowing eyes
        setCircle(21, 22, 7, sf::Color(255, 30, 0));
        setCircle(21, 22, 5, sf::Color(255, 110, 0));
        setCircle(21, 22, 2, sf::Color(255, 230, 80));
        setCircle(43, 22, 7, sf::Color(255, 30, 0));
        setCircle(43, 22, 5, sf::Color(255, 110, 0));
        setCircle(43, 22, 2, sf::Color(255, 230, 80));

        // Jagged snarling mouth
        setRect(19, 35, 45, 39, sf::Color(255, 15, 0));
        setRect(21, 39, 43, 42, sf::Color(210, 5, 0));
        // Upper fangs
        for (int i = 0; i < 5; ++i) {
            setRect(21 + i * 4, 31, 23 + i * 4, 36, sf::Color(255, 240, 200));
        }
        // Lower fangs
        for (int i = 0; i < 4; ++i) {
            setRect(23 + i * 4, 42, 25 + i * 4, 47, sf::Color(255, 240, 200));
        }

        // Side cannon pods
        setRect(2,  28, 10, 42, sf::Color(110, 15, 15));
        setRect(54, 28, 62, 42, sf::Color(110, 15, 15));
        // Cannon barrels
        setRect(3,  32, 8,  34, sf::Color(80, 10, 10));
        setRect(56, 32, 61, 34, sf::Color(80, 10, 10));
        // Cannon muzzle glow
        setCircle(5,  35, 4, sf::Color(255, 80, 0, 200));
        setCircle(59, 35, 4, sf::Color(255, 80, 0, 200));

        // Engine fire aura
        setRect(18, 50, 46, 56, sf::Color(255, 70, 0, 180));
        setRect(22, 56, 42, 62, sf::Color(255, 140, 0, 110));
        setRect(26, 61, 38, 64, sf::Color(255, 200, 50, 60));

        // Armor rivets
        for (int i = 0; i < 4; ++i) {
            setCircle(18 + i * 9, 15, 2, sf::Color(75, 10, 10));
        }
    }

    baseScaleX = 1.0f;
    baseScaleY = 1.0f;
    flashTimer = 0.0f;

    texture.create(W, H);
    texture.update(img);
    sprite.setTexture(texture, true);
    sprite.setOrigin(W / 2.0f, H / 2.0f);
}

void Enemy::promptUserUpload() {
    std::cout << "Enemy sprites are rendered in code - no external images needed." << std::endl;
}
