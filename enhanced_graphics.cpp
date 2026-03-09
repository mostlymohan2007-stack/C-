// Enhanced Graphics Features for Space Battle Game
// Add these features to your existing game for better visuals

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>

// Particle System for Explosions
class Particle {
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color color;
    float lifetime;
    float size;
    
    Particle(sf::Vector2f pos, sf::Vector2f vel, sf::Color col, float life) {
        position = pos;
        velocity = vel;
        color = col;
        lifetime = life;
        size = 3.0f;
    }
    
    void update(float deltaTime) {
        position += velocity * deltaTime;
        lifetime -= deltaTime;
        size *= 0.98f; // Shrink over time
    }
    
    bool isAlive() const {
        return lifetime > 0;
    }
};

class ParticleSystem {
private:
    std::vector<Particle> particles;
    
public:
    void createExplosion(sf::Vector2f position, sf::Color color = sf::Color::Orange) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> angleDist(0, 2 * 3.14159f);
        std::uniform_real_distribution<float> speedDist(50, 200);
        std::uniform_real_distribution<float> lifeDist(0.3f, 1.0f);
        
        for (int i = 0; i < 20; ++i) {
            float angle = angleDist(gen);
            float speed = speedDist(gen);
            float life = lifeDist(gen);
            
            sf::Vector2f velocity(
                cos(angle) * speed,
                sin(angle) * speed
            );
            
            particles.emplace_back(position, velocity, color, life);
        }
    }
    
    void update(float deltaTime) {
        for (auto& particle : particles) {
            particle.update(deltaTime);
        }
        
        // Remove dead particles
        particles.erase(
            std::remove_if(particles.begin(), particles.end(),
                [](const Particle& p) { return !p.isAlive(); }),
            particles.end()
        );
    }
    
    void render(sf::RenderWindow& window) {
        sf::CircleShape shape;
        for (const auto& particle : particles) {
            shape.setRadius(particle.size);
            shape.setFillColor(particle.color);
            shape.setPosition(particle.position);
            window.draw(shape);
        }
    }
};

// Star Background
class StarField {
private:
    struct Star {
        sf::Vector2f position;
        float brightness;
        float twinkleSpeed;
        float twinklePhase;
    };
    
    std::vector<Star> stars;
    int width, height;
    
public:
    StarField(int w, int h, int starCount = 100) : width(w), height(h) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> xDist(0, w);
        std::uniform_real_distribution<float> yDist(0, h);
        std::uniform_real_distribution<float> brightnessDist(0.3f, 1.0f);
        std::uniform_real_distribution<float> twinkleDist(1.0f, 3.0f);
        std::uniform_real_distribution<float> phaseDist(0, 2 * 3.14159f);
        
        for (int i = 0; i < starCount; ++i) {
            Star star;
            star.position = sf::Vector2f(xDist(gen), yDist(gen));
            star.brightness = brightnessDist(gen);
            star.twinkleSpeed = twinkleDist(gen);
            star.twinklePhase = phaseDist(gen);
            stars.push_back(star);
        }
    }
    
    void update(float deltaTime) {
        for (auto& star : stars) {
            star.twinklePhase += star.twinkleSpeed * deltaTime;
        }
    }
    
    void render(sf::RenderWindow& window) {
        sf::CircleShape starShape;
        for (const auto& star : stars) {
            float currentBrightness = star.brightness * (0.7f + 0.3f * sin(star.twinklePhase));
            starShape.setRadius(1.0f);
            starShape.setFillColor(sf::Color(
                static_cast<sf::Uint8>(255 * currentBrightness),
                static_cast<sf::Uint8>(255 * currentBrightness),
                static_cast<sf::Uint8>(255 * currentBrightness)
            ));
            starShape.setPosition(star.position);
            window.draw(starShape);
        }
    }
};

// Health Bar
class HealthBar {
private:
    sf::RectangleShape background;
    sf::RectangleShape fill;
    float maxWidth;
    
public:
    HealthBar(float x, float y, float width, float height) : maxWidth(width) {
        background.setSize(sf::Vector2f(width, height));
        background.setFillColor(sf::Color(100, 100, 100));
        background.setPosition(x, y);
        
        fill.setSize(sf::Vector2f(width, height));
        fill.setFillColor(sf::Color::Green);
        fill.setPosition(x, y);
    }
    
    void setHealth(float healthPercent) {
        fill.setSize(sf::Vector2f(maxWidth * healthPercent, fill.getSize().y));
        
        // Change color based on health
        if (healthPercent > 0.6f) {
            fill.setFillColor(sf::Color::Green);
        } else if (healthPercent > 0.3f) {
            fill.setFillColor(sf::Color::Yellow);
        } else {
            fill.setFillColor(sf::Color::Red);
        }
    }
    
    void render(sf::RenderWindow& window) {
        window.draw(background);
        window.draw(fill);
    }
};

// Power-up System
class PowerUp {
public:
    enum Type {
        RAPID_FIRE,
        TRIPLE_SHOT,
        SHIELD,
        HEALTH
    };
    
private:
    sf::Sprite sprite;
    Type type;
    sf::Vector2f position;
    float lifetime;
    bool active;
    
public:
    PowerUp(Type t, sf::Vector2f pos) : type(t), position(pos), lifetime(10.0f), active(true) {
        // Load different colors for different power-ups
        sf::Texture texture;
        texture.create(20, 20);
        sf::Image image;
        
        switch (type) {
            case RAPID_FIRE:
                image.create(20, 20, sf::Color::Cyan);
                break;
            case TRIPLE_SHOT:
                image.create(20, 20, sf::Color::Magenta);
                break;
            case SHIELD:
                image.create(20, 20, sf::Color::Blue);
                break;
            case HEALTH:
                image.create(20, 20, sf::Color::Green);
                break;
        }
        
        texture.update(image);
        sprite.setTexture(texture);
        sprite.setPosition(position);
    }
    
    void update(float deltaTime) {
        lifetime -= deltaTime;
        if (lifetime <= 0) {
            active = false;
        }
    }
    
    void render(sf::RenderWindow& window) {
        if (active) {
            window.draw(sprite);
        }
    }
    
    bool isActive() const { return active; }
    Type getType() const { return type; }
    sf::Vector2f getPosition() const { return position; }
    sf::FloatRect getBounds() const { return sprite.getGlobalBounds(); }
};

// Sound Effects (if you want to add audio)
class SoundManager {
private:
    sf::SoundBuffer shootBuffer;
    sf::SoundBuffer explosionBuffer;
    sf::Sound shootSound;
    sf::Sound explosionSound;
    
public:
    bool loadSounds() {
        // Try to load sound files, return false if not found
        bool success = true;
        
        if (!shootBuffer.loadFromFile("assets/shoot.wav")) {
            success = false;
        }
        if (!explosionBuffer.loadFromFile("assets/explosion.wav")) {
            success = false;
        }
        
        if (success) {
            shootSound.setBuffer(shootBuffer);
            explosionSound.setBuffer(explosionBuffer);
        }
        
        return success;
    }
    
    void playShoot() {
        if (shootBuffer.getSampleCount() > 0) {
            shootSound.play();
        }
    }
    
    void playExplosion() {
        if (explosionBuffer.getSampleCount() > 0) {
            explosionSound.play();
        }
    }
};

// Usage Instructions:
/*
1. Add these classes to your Game.h and Game.cpp
2. Initialize in Game constructor:
   - particleSystem
   - starField
   - healthBar
   - soundManager

3. Update in Game::update():
   - particleSystem.update(deltaTime);
   - starField.update(deltaTime);

4. Render in Game::render():
   - starField.render(window);
   - particleSystem.render(window);
   - healthBar.render(window);

5. Create explosions when enemies are destroyed:
   - particleSystem.createExplosion(enemy.getPosition());

6. Add power-ups randomly:
   - When enemies are destroyed, chance to spawn power-up

7. Add sound effects:
   - Call soundManager.playShoot() when firing
   - Call soundManager.playExplosion() on explosions
*/
