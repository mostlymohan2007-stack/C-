#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <thread>
#include <chrono>

// Cross-platform space battle game for online compilers
class OnlineSpaceBattle {
private:
    int playerX, playerY;
    int score;
    int health;
    bool gameRunning;
    int width, height;
    
    struct Enemy {
        int x, y;
        bool alive;
        char symbol;
    };
    
    struct Bullet {
        int x, y;
        bool active;
        bool isPlayer;
        char symbol;
    };
    
    std::vector<Enemy> enemies;
    std::vector<Bullet> bullets;
    
public:
    OnlineSpaceBattle(int w = 50, int h = 25) : 
        playerX(w/2), playerY(h-3), score(0), health(100), 
        gameRunning(true), width(w), height(h) {
        srand(time(nullptr));
    }
    
    void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }
    
    void draw() {
        clearScreen();
        
        // Draw header
        std::cout << "╔";
        for (int i = 0; i < width; i++) std::cout << "═";
        std::cout << "╗" << std::endl;
        
        // Draw game area
        for (int y = 0; y < height; y++) {
            std::cout << "║";
            for (int x = 0; x < width; x++) {
                char displayChar = ' ';
                
                // Draw player
                if (x == playerX && y == playerY) {
                    displayChar = '▲';
                }
                else {
                    // Draw enemies
                    for (const auto& enemy : enemies) {
                        if (enemy.alive && x == enemy.x && y == enemy.y) {
                            displayChar = enemy.symbol;
                            break;
                        }
                    }
                    
                    // Draw bullets
                    if (displayChar == ' ') {
                        for (const auto& bullet : bullets) {
                            if (bullet.active && x == bullet.x && y == bullet.y) {
                                displayChar = bullet.symbol;
                                break;
                            }
                        }
                    }
                }
                
                std::cout << displayChar;
            }
            std::cout << "║" << std::endl;
        }
        
        // Draw footer
        std::cout << "╚";
        for (int i = 0; i < width; i++) std::cout << "═";
        std::cout << "╝" << std::endl;
        
        // Draw stats
        std::cout << "Score: " << score << "  |  Health: " << health << "  |  Enemies: " << enemies.size() << std::endl;
        std::cout << "Controls: A/D=Move, W/S=Up/Down, Space=Shoot, Q=Quit" << std::endl;
    }
    
    void spawnEnemy() {
        Enemy enemy;
        enemy.x = rand() % (width - 2) + 1;
        enemy.y = 1;
        enemy.alive = true;
        
        // Random enemy types
        int type = rand() % 3;
        switch(type) {
            case 0: enemy.symbol = '●'; break;  // Basic enemy
            case 1: enemy.symbol = '■'; break;  // Heavy enemy  
            case 2: enemy.symbol = '◆'; break;  // Fast enemy
        }
        
        enemies.push_back(enemy);
    }
    
    void shoot() {
        Bullet bullet;
        bullet.x = playerX;
        bullet.y = playerY - 1;
        bullet.active = true;
        bullet.isPlayer = true;
        bullet.symbol = '|';
        bullets.push_back(bullet);
    }
    
    void update() {
        // Move bullets
        for (auto& bullet : bullets) {
            if (bullet.active) {
                if (bullet.isPlayer) {
                    bullet.y--;
                    if (bullet.y < 0) bullet.active = false;
                } else {
                    bullet.y++;
                    if (bullet.y >= height) bullet.active = false;
                }
            }
        }
        
        // Move enemies (simple downward movement)
        for (auto& enemy : enemies) {
            if (enemy.alive) {
                // Random movement pattern
                if (rand() % 10 == 0) {
                    int move = rand() % 3 - 1; // -1, 0, or 1
                    enemy.x += move;
                    enemy.x = std::max(1, std::min(width - 2, enemy.x));
                }
                
                enemy.y++;
                if (enemy.y >= height) {
                    enemy.alive = false;
                    health -= 15;
                }
                
                // Check collision with player
                if (enemy.x == playerX && enemy.y == playerY) {
                    enemy.alive = false;
                    health -= 25;
                }
                
                // Enemy shooting
                if (rand() % 50 == 0) {
                    Bullet bullet;
                    bullet.x = enemy.x;
                    bullet.y = enemy.y + 1;
                    bullet.active = true;
                    bullet.isPlayer = false;
                    bullet.symbol = '↓';
                    bullets.push_back(bullet);
                }
            }
        }
        
        // Check bullet-enemy collisions
        for (auto& bullet : bullets) {
            if (bullet.active && bullet.isPlayer) {
                for (auto& enemy : enemies) {
                    if (enemy.alive && bullet.x == enemy.x && bullet.y == enemy.y) {
                        bullet.active = false;
                        enemy.alive = false;
                        score += (enemy.symbol == '●') ? 10 : 
                                (enemy.symbol == '■') ? 20 : 15;
                    }
                }
            }
        }
        
        // Check bullet-player collisions
        for (auto& bullet : bullets) {
            if (bullet.active && !bullet.isPlayer) {
                if (bullet.x == playerX && bullet.y == playerY) {
                    bullet.active = false;
                    health -= 10;
                }
            }
        }
        
        // Remove inactive objects
        enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
            [](const Enemy& e) { return !e.alive; }), enemies.end());
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b) { return !b.active; }), bullets.end());
        
        // Check game over
        if (health <= 0) {
            gameRunning = false;
        }
    }
    
    void handleInput() {
        std::cout << "Enter command (a/d/w/s/space/q): ";
        std::string input;
        std::getline(std::cin, input);
        
        if (!input.empty()) {
            char cmd = tolower(input[0]);
            switch (cmd) {
                case 'a': // Left
                    if (playerX > 1) playerX--;
                    break;
                case 'd': // Right
                    if (playerX < width - 2) playerX++;
                    break;
                case 'w': // Up
                    if (playerY > 1) playerY--;
                    break;
                case 's': // Down
                    if (playerY < height - 2) playerY++;
                    break;
                case ' ': // Shoot
                    shoot();
                    break;
                case 'q': // Quit
                    gameRunning = false;
                    break;
            }
        }
    }
    
    void run() {
        int frameCount = 0;
        
        std::cout << "🚀 SPACE BATTLE GAME 🚀\n\n";
        std::cout << "Welcome Commander! Defend Earth from the alien invasion!\n\n";
        std::cout << "Controls:\n";
        std::cout << "  A/D - Move left/right\n";
        std::cout << "  W/S - Move up/down\n";
        std::cout << "  Space - Shoot\n";
        std::cout << "  Q - Quit\n\n";
        std::cout << "Press Enter to start...\n";
        std::cin.get();
        
        while (gameRunning && health > 0) {
            draw();
            
            if (frameCount % 20 == 0) {
                spawnEnemy();
            }
            
            // Auto-update for some movement
            if (frameCount % 2 == 0) {
                update();
            }
            
            handleInput();
            frameCount++;
        }
        
        clearScreen();
        std::cout << "🎮 GAME OVER 🎮\n\n";
        std::cout << "Final Score: " << score << std::endl;
        std::cout << "Health Remaining: " << health << std::endl;
        
        if (score >= 100) {
            std::cout << "🏆 EXCELLENT! You're a space hero!\n";
        } else if (score >= 50) {
            std::cout << "⭐ GOOD JOB! You fought well!\n";
        } else {
            std::cout << "💪 Keep practicing, commander!\n";
        }
        
        std::cout << "\nThanks for playing!\n";
    }
};

int main() {
    OnlineSpaceBattle game;
    game.run();
    return 0;
}
