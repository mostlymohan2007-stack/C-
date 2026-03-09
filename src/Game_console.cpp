#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <windows.h>

// Simple console-based space battle game
class ConsoleGame {
private:
    int playerX, playerY;
    int score;
    int health;
    bool gameRunning;
    
    struct Enemy {
        int x, y;
        bool alive;
    };
    
    struct Bullet {
        int x, y;
        bool active;
        bool isPlayer;
    };
    
    std::vector<Enemy> enemies;
    std::vector<Bullet> bullets;
    
public:
    ConsoleGame() : playerX(40), playerY(20), score(0), health(100), gameRunning(true) {
        srand(time(nullptr));
        system("cls");
    }
    
    void clearScreen() {
        COORD cursorPosition;
        cursorPosition.X = 0;
        cursorPosition.Y = 0;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);
    }
    
    void draw() {
        clearScreen();
        
        // Draw game border
        std::cout << "+----------------------------------------+\n";
        for (int y = 0; y < 22; y++) {
            std::cout << "|";
            for (int x = 0; x < 40; x++) {
                if (x == playerX && y == playerY) {
                    std::cout << "A"; // Player spaceship
                }
                else {
                    bool drawn = false;
                    
                    // Draw enemies
                    for (const auto& enemy : enemies) {
                        if (enemy.alive && x == enemy.x && y == enemy.y) {
                            std::cout << "E"; // Enemy
                            drawn = true;
                            break;
                        }
                    }
                    
                    // Draw bullets
                    if (!drawn) {
                        for (const auto& bullet : bullets) {
                            if (bullet.active && x == bullet.x && y == bullet.y) {
                                std::cout << (bullet.isPlayer ? "|" : "v"); // Bullet
                                drawn = true;
                                break;
                            }
                        }
                    }
                    
                    if (!drawn) std::cout << " ";
                }
            }
            std::cout << "|\n";
        }
        std::cout << "+----------------------------------------+\n";
        std::cout << "Score: " << score << "  Health: " << health << "\n";
        std::cout << "Arrow Keys: Move  Space: Shoot  ESC: Exit\n";
    }
    
    void spawnEnemy() {
        Enemy enemy;
        enemy.x = rand() % 38 + 1;
        enemy.y = 1;
        enemy.alive = true;
        enemies.push_back(enemy);
    }
    
    void shoot() {
        Bullet bullet;
        bullet.x = playerX;
        bullet.y = playerY - 1;
        bullet.active = true;
        bullet.isPlayer = true;
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
                    if (bullet.y > 21) bullet.active = false;
                }
            }
        }
        
        // Move enemies
        for (auto& enemy : enemies) {
            if (enemy.alive) {
                enemy.y++;
                if (enemy.y > 21) {
                    enemy.alive = false;
                    health -= 10;
                }
                
                // Check collision with player
                if (enemy.x == playerX && enemy.y == playerY) {
                    enemy.alive = false;
                    health -= 20;
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
                        score += 10;
                    }
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
        if (_kbhit()) {
            char key = _getch();
            switch (key) {
                case 75: // Left arrow
                    if (playerX > 1) playerX--;
                    break;
                case 77: // Right arrow
                    if (playerX < 38) playerX++;
                    break;
                case 72: // Up arrow
                    if (playerY > 1) playerY--;
                    break;
                case 80: // Down arrow
                    if (playerY < 20) playerY++;
                    break;
                case 32: // Space
                    shoot();
                    break;
                case 27: // ESC
                    gameRunning = false;
                    break;
            }
        }
    }
    
    void run() {
        int frameCount = 0;
        
        std::cout << "SPACE BATTLE GAME\n\n";
        std::cout << "Press any key to start...\n";
        _getch();
        
        while (gameRunning) {
            draw();
            handleInput();
            update();
            
            // Spawn enemies periodically
            frameCount++;
            if (frameCount % 30 == 0) {
                spawnEnemy();
            }
            
            Sleep(100); // Simple frame rate control
        }
        
        system("cls");
        std::cout << "GAME OVER\n";
        std::cout << "Final Score: " << score << "\n";
        std::cout << "Press any key to exit...\n";
        _getch();
    }
};

int main() {
    ConsoleGame game;
    game.run();
    return 0;
}
