#ifndef PARTICLE_H
#define PARTICLE_H

#include <SFML/Graphics.hpp>
#include <vector>

struct Particle {
    sf::CircleShape shape;
    sf::Vector2f velocity;
    float lifetime;
    float maxLifetime;
    sf::Color baseColor;
};

class ParticleSystem {
public:
    ParticleSystem() {}

    void spawnExplosion(float x, float y, sf::Color color, int count = 18) {
        for (int i = 0; i < count; ++i) {
            Particle p;
            float angle = (i / (float)count) * 360.0f * 3.14159f / 180.0f;
            float speed = 60.0f + (rand() % 140);
            p.velocity = { std::cos(angle) * speed, std::sin(angle) * speed };
            float size = 2.0f + (rand() % 4);
            p.shape.setRadius(size);
            p.shape.setOrigin(size, size);
            p.shape.setPosition(x, y);
            p.baseColor = color;
            p.shape.setFillColor(color);
            p.lifetime = 0.5f + (rand() % 40) / 100.0f;
            p.maxLifetime = p.lifetime;
            particles.push_back(p);
        }
    }

    void spawnPowerUpGlow(float x, float y, sf::Color color) {
        for (int i = 0; i < 6; ++i) {
            Particle p;
            float angle = (rand() % 360) * 3.14159f / 180.0f;
            float speed = 20.0f + (rand() % 30);
            p.velocity = { std::cos(angle) * speed, std::sin(angle) * speed - 30.0f };
            float size = 1.5f + (rand() % 3);
            p.shape.setRadius(size);
            p.shape.setOrigin(size, size);
            p.shape.setPosition(x, y);
            p.baseColor = color;
            p.shape.setFillColor(color);
            p.lifetime = 0.4f;
            p.maxLifetime = p.lifetime;
            particles.push_back(p);
        }
    }

    void update(float dt) {
        for (auto& p : particles) {
            p.lifetime -= dt;
            p.velocity.y += 30.0f * dt; // slight gravity
            p.shape.move(p.velocity.x * dt, p.velocity.y * dt);
            float alpha = std::max(0.0f, p.lifetime / p.maxLifetime);
            sf::Color c = p.baseColor;
            c.a = static_cast<sf::Uint8>(alpha * 255);
            p.shape.setFillColor(c);
        }
        particles.erase(
            std::remove_if(particles.begin(), particles.end(),
                [](const Particle& p) { return p.lifetime <= 0.0f; }),
            particles.end()
        );
    }

    void render(sf::RenderWindow& window) {
        for (auto& p : particles) {
            window.draw(p.shape);
        }
    }

private:
    std::vector<Particle> particles;
};

#endif // PARTICLE_H
