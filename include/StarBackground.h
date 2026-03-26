#ifndef STARBACKGROUND_H
#define STARBACKGROUND_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>

struct Star {
    sf::CircleShape shape;
    float speed;
    float twinkleTimer;
    float twinkleRate;
    sf::Color baseColor;
};

class StarBackground {
public:
    StarBackground(float screenW, float screenH) {
        // Three layers for parallax depth
        for (int i = 0; i < 80; ++i) {
            Star s;
            float size = 0.5f + (rand() % 15) / 10.0f;
            s.shape.setRadius(size);
            s.shape.setPosition(
                static_cast<float>(rand() % static_cast<int>(screenW)),
                static_cast<float>(rand() % static_cast<int>(screenH))
            );
            // Vary brightness
            int bright = 120 + rand() % 135;
            // Occasional colored stars
            int colorRoll = rand() % 10;
            if (colorRoll < 2)
                s.baseColor = sf::Color(bright, bright, 255); // blue
            else if (colorRoll < 4)
                s.baseColor = sf::Color(255, bright, bright); // red giant
            else
                s.baseColor = sf::Color(bright, bright, bright); // white
            s.shape.setFillColor(s.baseColor);
            s.speed = 15.0f + size * 20.0f;  // closer = faster
            s.twinkleTimer = static_cast<float>(rand() % 100) / 100.0f;
            s.twinkleRate = 0.5f + static_cast<float>(rand() % 100) / 100.0f;
            stars.push_back(s);
        }
        width = screenW;
        height = screenH;
    }

    void update(float dt) {
        for (auto& s : stars) {
            s.shape.move(0, s.speed * dt);
            if (s.shape.getPosition().y > height + 5.0f) {
                s.shape.setPosition(static_cast<float>(rand() % static_cast<int>(width)), -5.0f);
            }
            // Twinkle
            s.twinkleTimer += dt * s.twinkleRate;
            float alpha = 160.0f + 95.0f * std::sin(s.twinkleTimer * 3.14159f);
            sf::Color c = s.baseColor;
            c.a = static_cast<sf::Uint8>(alpha);
            s.shape.setFillColor(c);
        }
    }

    void render(sf::RenderWindow& window) {
        for (auto& s : stars) {
            window.draw(s.shape);
        }
    }

private:
    std::vector<Star> stars;
    float width, height;
};

#endif // STARBACKGROUND_H
