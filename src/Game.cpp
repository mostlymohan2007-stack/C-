#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <sstream>
#include <iomanip>

// ============================================================
// CONSTRUCTOR
// ============================================================
Game::Game()
    : window(sf::VideoMode(900, 700), "Space Battle - Enhanced Edition",
             sf::Style::Titlebar | sf::Style::Close),
      currentState(GameState::START),
      currentLevel(Level::LEVEL_1),
      score(0), enemySpawnTimer(0.0f), difficultyMultiplier(1.0f),
      rapidFireTimer(0.0f), hasRapidFire(false), powerUpSpawnChance(0.35f),
      shakeTimer(0.0f), shakeMagnitude(0.0f),
      animTimer(0.0f), titlePulse(0.0f),
      fpsTimer(0.0f), frameCount(0), currentFPS(60.0f)
{
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    srand(static_cast<unsigned int>(time(nullptr)));

    // Star background
    starBg = std::make_unique<StarBackground>(900.0f, 700.0f);

    // Initialize sound manager and start background music
    soundManager.initialize();
    soundManager.playBackgroundMusic(true);

    initializeLevels();
    setupLevel(Level::LEVEL_1);
    setupUI();

    player.setPosition(450.0f, 580.0f);
}

// ============================================================
// RUN LOOP
// ============================================================
void Game::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        // Cap deltaTime to prevent spiral of death
        if (deltaTime > 0.05f) deltaTime = 0.05f;

        animTimer += deltaTime;
        titlePulse += deltaTime;

        // FPS counter
        frameCount++;
        fpsTimer += deltaTime;
        if (fpsTimer >= 1.0f) {
            currentFPS = static_cast<float>(frameCount) / fpsTimer;
            frameCount = 0;
            fpsTimer = 0.0f;
        }

        processEvents();

        if (currentState == GameState::PLAYING) {
            update(deltaTime);
        } else {
            // Still update stars on menus
            starBg->update(deltaTime);
            particles.update(deltaTime);
        }

        render();
    }
}

// ============================================================
// PROCESS EVENTS
// ============================================================
void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                window.close();
            }

            if (currentState == GameState::START ||
                currentState == GameState::GAME_OVER ||
                currentState == GameState::WIN) {
                if (event.key.code == sf::Keyboard::Space) {
                    score = 0;
                    currentLevel = Level::LEVEL_1;
                    enemySpawnTimer = 0.0f;
                    enemies.clear();
                    bullets.clear();
                    powerUps.clear();
                    hasRapidFire = false;
                    rapidFireTimer = 0.0f;
                    player.setPosition(450.0f, 580.0f);
                    player.heal(player.getMaxHealth());
                    setupLevel(Level::LEVEL_1);
                    currentState = GameState::PLAYING;
                }
            }
            else if (currentState == GameState::LEVEL_COMPLETE) {
                if (event.key.code == sf::Keyboard::Space) {
                    nextLevel();
                    currentState = GameState::PLAYING;
                }
            }
        }
    }

    // Continuous input during gameplay
    if (currentState == GameState::PLAYING) {
        const float moveSpeed = 240.0f;
        const float dt = 1.0f / 60.0f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            float nx = player.getBounds().left - moveSpeed * dt;
            if (nx > -10) player.move(-moveSpeed * dt, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            float rx = player.getBounds().left + player.getBounds().width + moveSpeed * dt;
            if (rx < 910) player.move(moveSpeed * dt, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            float ny = player.getBounds().top - moveSpeed * dt;
            if (ny > 0) player.move(0, -moveSpeed * dt);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            float dy = player.getBounds().top + player.getBounds().height + moveSpeed * dt;
            if (dy < 700) player.move(0, moveSpeed * dt);
        }

        // Shoot: Space, LCtrl, or RCtrl
        bool wantShoot = sf::Keyboard::isKeyPressed(sf::Keyboard::Space) ||
                         sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
                         sf::Keyboard::isKeyPressed(sf::Keyboard::RControl) ||
                         sf::Keyboard::isKeyPressed(sf::Keyboard::Z);

        if (wantShoot && player.canShoot()) {
            sf::FloatRect pb = player.getBounds();
            float bx = pb.left + pb.width / 2.0f;
            float by = pb.top - 5.0f;
            if (hasRapidFire) {
                // Triple shot during rapid fire
                bullets.emplace_back(BulletType::PLAYER, bx,      by, 0.0f,   -1.0f);
                bullets.emplace_back(BulletType::PLAYER, bx - 12, by, -0.12f, -1.0f);
                bullets.emplace_back(BulletType::PLAYER, bx + 12, by,  0.12f, -1.0f);
            } else {
                bullets.emplace_back(BulletType::PLAYER, bx, by, 0.0f, -1.0f);
            }
            player.resetFireTimer();
            soundManager.playSound(SoundType::PLAYER_SHOOT);
        }
    }
}

// ============================================================
// UPDATE
// ============================================================
void Game::update(float deltaTime) {
    starBg->update(deltaTime);
    particles.update(deltaTime);
    updateScreenShake(deltaTime);

    player.update(deltaTime);
    updatePowerUps(deltaTime);

    // Update enemies + enemy shooting
    for (auto& enemy : enemies) {
        enemy.update(deltaTime);
        if (enemy.shouldShoot()) {
            sf::FloatRect eb = enemy.getBounds();
            float bx = eb.left + eb.width / 2.0f;
            float by = eb.top + eb.height;
            bullets.emplace_back(BulletType::ENEMY, bx, by, 0.0f, 1.0f);
            enemy.resetFireTimer();
            soundManager.playSound(SoundType::ENEMY_SHOOT);
        }
    }

    for (auto& bullet : bullets) {
        bullet.update(deltaTime);
    }

    // Spawn enemies
    enemySpawnTimer += deltaTime;
    const auto& config = levelConfigs[currentLevel];
    if (enemySpawnTimer >= config.enemySpawnRate) {
        spawnEnemy();
        enemySpawnTimer = 0.0f;
    }

    handleCollisions();

    // Clean up dead enemies
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [](const Enemy& e) { return !e.isAlive(); }), enemies.end());

    // Clean up old bullets
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [](const Bullet& b) { return !b.isActive() || b.isOutOfBounds(900, 700); }), bullets.end());

    if (!player.isAlive()) {
        currentState = GameState::GAME_OVER;
        // Big explosion on player death
        sf::FloatRect pb = player.getBounds();
        particles.spawnExplosion(pb.left + pb.width/2, pb.top + pb.height/2,
                                  sf::Color(50, 100, 255), 35);
        triggerScreenShake(12.0f, 0.8f);
        soundManager.playSound(SoundType::GAME_OVER);
        soundManager.stopBackgroundMusic();
    }

    checkLevelComplete();
    updateUI();
}

// ============================================================
// RENDER
// ============================================================
void Game::render() {
    // Apply screen shake offset
    sf::View view = window.getDefaultView();
    view.move(cameraOffset);
    window.setView(view);

    window.clear(sf::Color(5, 5, 20));

    // Draw star background
    starBg->render(window);

    // Nebula overlay
    window.draw(nebulaLayer);

    if (currentState == GameState::START) {
        renderStartScreen();
    }
    else if (currentState == GameState::PLAYING) {
        // Draw power-ups
        for (auto& pu : powerUps) pu.render(window);

        // Draw enemies
        for (auto& e : enemies) e.render(window);

        // Draw bullets
        for (auto& b : bullets) b.render(window);

        // Draw player
        player.render(window);

        // Draw particles
        particles.render(window);

        // Draw HUD
        renderHUD();
    }
    else if (currentState == GameState::LEVEL_COMPLETE) {
        renderLevelComplete();
    }
    else if (currentState == GameState::GAME_OVER) {
        particles.render(window);
        renderGameOverScreen();
    }
    else if (currentState == GameState::WIN) {
        renderWinScreen();
    }

    // Reset view for HUD (unshaken)
    window.setView(window.getDefaultView());

    window.display();
}

// ============================================================
// RENDER SCREENS
// ============================================================
void Game::renderStartScreen() {
    // Animated gradient panel
    sf::RectangleShape panel(sf::Vector2f(700, 450));
    panel.setPosition(100, 120);
    float alpha = 170 + static_cast<int>(25 * std::sin(titlePulse * 1.5f));
    panel.setFillColor(sf::Color(10, 10, 40, static_cast<sf::Uint8>(alpha)));
    panel.setOutlineColor(sf::Color(60, 140, 255, 200));
    panel.setOutlineThickness(2.5f);
    window.draw(panel);

    // Title with pulsing glow color
    float pulse = std::sin(titlePulse * 2.5f);
    sf::Uint8 r = static_cast<sf::Uint8>(50 + 60 * pulse);
    sf::Uint8 g = static_cast<sf::Uint8>(180 + 60 * pulse);

    gameStateText.setString("SPACE BATTLE");
    gameStateText.setCharacterSize(64);
    gameStateText.setFillColor(sf::Color(r, g, 255));
    gameStateText.setStyle(sf::Text::Bold);
    sf::FloatRect tb = gameStateText.getLocalBounds();
    gameStateText.setOrigin(tb.width / 2, tb.height / 2);
    gameStateText.setPosition(450, 190);
    window.draw(gameStateText);

    subtitleText.setString("Enhanced Edition");
    subtitleText.setFillColor(sf::Color(150, 220, 255, 200));
    subtitleText.setCharacterSize(22);
    sf::FloatRect sb = subtitleText.getLocalBounds();
    subtitleText.setOrigin(sb.width / 2, sb.height / 2);
    subtitleText.setPosition(450, 248);
    window.draw(subtitleText);

    // Enemy emoji legend
    sf::Text legend;
    legend.setFont(font);
    legend.setCharacterSize(18);
    legend.setFillColor(sf::Color(200, 200, 255));
    legend.setPosition(170, 290);
    legend.setString(
        "Level 1 :  Alien Invaders  (Basic)\n\n"
        "Level 2 :  Alien + Robot Drones  (Mixed)\n\n"
        "Level 3 :  Demon Overlords  (Heavy / Fast)\n\n"
        "Power-ups:   Rapid Fire  |  Health Boost"
    );
    window.draw(legend);

    // Controls
    controlsText.setString(
        "WASD / Arrow Keys  :  Move\n"
        "SPACE / CTRL / Z   :  SHOOT\n"
        "ESC                :  Exit"
    );
    controlsText.setCharacterSize(17);
    controlsText.setFillColor(sf::Color(180, 255, 180));
    sf::FloatRect cb = controlsText.getLocalBounds();
    controlsText.setOrigin(cb.width / 2, 0);
    controlsText.setPosition(450, 450);
    window.draw(controlsText);

    // Pulsing start prompt
    sf::Uint8 pa = static_cast<sf::Uint8>(150 + 105 * std::abs(std::sin(titlePulse * 3.0f)));
    sf::Text startPrompt;
    startPrompt.setFont(font);
    startPrompt.setCharacterSize(26);
    startPrompt.setFillColor(sf::Color(255, 220, 50, pa));
    startPrompt.setStyle(sf::Text::Bold);
    startPrompt.setString(">> PRESS SPACE TO START <<");
    sf::FloatRect spb = startPrompt.getLocalBounds();
    startPrompt.setOrigin(spb.width / 2, 0);
    startPrompt.setPosition(450, 570);
    window.draw(startPrompt);
}

void Game::renderHUD() {
    // HUD panel
    window.draw(hudPanel);

    // Score
    scoreText.setString("SCORE: " + std::to_string(score));
    window.draw(scoreText);

    // Level
    std::string lvlStr = "LVL " + std::to_string(static_cast<int>(currentLevel) + 1);
    levelText.setString(lvlStr);
    window.draw(levelText);

    // Target
    targetText.setString("TARGET: " + std::to_string(targetScore));
    window.draw(targetText);

    // Health bar
    drawHealthBar();

    // Rapid fire badge
    if (hasRapidFire) {
        float t = std::abs(std::sin(animTimer * 5.0f));
        sf::Uint8 ga = static_cast<sf::Uint8>(180 + 75 * t);
        powerUpStatus.setString("RAPID FIRE! [" + std::to_string(static_cast<int>(rapidFireTimer) + 1) + "s]");
        powerUpStatus.setFillColor(sf::Color(255, 230, 30, ga));
        window.draw(powerUpStatus);
    }

    // FPS (top right)
    std::ostringstream fpsSS;
    fpsSS << std::fixed << std::setprecision(0) << currentFPS << " FPS";
    fpsText.setString(fpsSS.str());
    window.draw(fpsText);
}

void Game::drawHealthBar() {
    float maxW = 200.0f;
    float ratio = static_cast<float>(player.getHealth()) / static_cast<float>(player.getMaxHealth());
    float barW = maxW * ratio;

    healthBarBg.setSize(sf::Vector2f(maxW, 16));
    healthBarBg.setPosition(690, 20);
    healthBarBg.setFillColor(sf::Color(40, 0, 0, 200));
    window.draw(healthBarBg);

    // Color gradient: green -> yellow -> red
    sf::Color barColor;
    if (ratio > 0.5f) {
        float t = (ratio - 0.5f) * 2.0f;
        barColor = sf::Color(
            static_cast<sf::Uint8>(255 * (1.0f - t)),
            static_cast<sf::Uint8>(200 + 55 * t),
            50
        );
    } else {
        float t = ratio * 2.0f;
        barColor = sf::Color(255, static_cast<sf::Uint8>(200 * t), 50);
    }

    healthBarFg.setSize(sf::Vector2f(barW, 16));
    healthBarFg.setPosition(690, 20);
    healthBarFg.setFillColor(barColor);
    window.draw(healthBarFg);

    // Animated shimmer on bar
    float shimmer = std::abs(std::sin(animTimer * 4.0f));
    sf::RectangleShape shimmerBar(sf::Vector2f(barW, 5));
    shimmerBar.setPosition(690, 20);
    shimmerBar.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(40 * shimmer)));
    window.draw(shimmerBar);

    healthBarBorder.setSize(sf::Vector2f(maxW + 2, 18));
    healthBarBorder.setPosition(689, 19);
    healthBarBorder.setFillColor(sf::Color::Transparent);
    healthBarBorder.setOutlineColor(sf::Color(100, 200, 255, 180));
    healthBarBorder.setOutlineThickness(1.5f);
    window.draw(healthBarBorder);

    // Health label
    healthLabel.setString("HP " + std::to_string(player.getHealth()) + "/" + std::to_string(player.getMaxHealth()));
    window.draw(healthLabel);
}

void Game::renderLevelComplete() {
    sf::RectangleShape overlay(sf::Vector2f(900, 700));
    overlay.setFillColor(sf::Color(0, 0, 0, 130));
    window.draw(overlay);

    sf::RectangleShape panel(sf::Vector2f(600, 300));
    panel.setPosition(150, 200);
    panel.setFillColor(sf::Color(10, 30, 10, 220));
    panel.setOutlineColor(sf::Color(50, 255, 100, 220));
    panel.setOutlineThickness(2.5f);
    window.draw(panel);

    gameStateText.setCharacterSize(52);
    gameStateText.setStyle(sf::Text::Bold);
    gameStateText.setFillColor(sf::Color(80, 255, 120));
    gameStateText.setString("LEVEL CLEARED!");
    sf::FloatRect lb = gameStateText.getLocalBounds();
    gameStateText.setOrigin(lb.width / 2, lb.height / 2);
    gameStateText.setPosition(450, 280);
    window.draw(gameStateText);

    subtitleText.setCharacterSize(26);
    subtitleText.setFillColor(sf::Color(200, 255, 200));
    subtitleText.setString("Score: " + std::to_string(score));
    sf::FloatRect slb = subtitleText.getLocalBounds();
    subtitleText.setOrigin(slb.width / 2, slb.height / 2);
    subtitleText.setPosition(450, 355);
    window.draw(subtitleText);

    sf::Uint8 pa = static_cast<sf::Uint8>(150 + 105 * std::abs(std::sin(titlePulse * 2.5f)));
    sf::Text prompt;
    prompt.setFont(font);
    prompt.setCharacterSize(22);
    prompt.setFillColor(sf::Color(255, 230, 50, pa));
    prompt.setString("PRESS SPACE to continue");
    sf::FloatRect pb = prompt.getLocalBounds();
    prompt.setOrigin(pb.width / 2, 0);
    prompt.setPosition(450, 420);
    window.draw(prompt);
}

void Game::renderGameOverScreen() {
    sf::RectangleShape overlay(sf::Vector2f(900, 700));
    overlay.setFillColor(sf::Color(0, 0, 0, 160));
    window.draw(overlay);

    sf::RectangleShape panel(sf::Vector2f(620, 320));
    panel.setPosition(140, 190);
    panel.setFillColor(sf::Color(30, 0, 0, 220));
    panel.setOutlineColor(sf::Color(255, 60, 60, 200));
    panel.setOutlineThickness(2.5f);
    window.draw(panel);

    gameStateText.setCharacterSize(60);
    gameStateText.setStyle(sf::Text::Bold);
    float p = std::sin(titlePulse * 3.0f);
    gameStateText.setFillColor(sf::Color(255, static_cast<sf::Uint8>(50 + 30*p), 50));
    gameStateText.setString("GAME OVER");
    sf::FloatRect lb = gameStateText.getLocalBounds();
    gameStateText.setOrigin(lb.width / 2, lb.height / 2);
    gameStateText.setPosition(450, 280);
    window.draw(gameStateText);

    subtitleText.setCharacterSize(24);
    subtitleText.setFillColor(sf::Color(255, 200, 200));
    subtitleText.setString("Final Score: " + std::to_string(score));
    sf::FloatRect slb = subtitleText.getLocalBounds();
    subtitleText.setOrigin(slb.width / 2, slb.height / 2);
    subtitleText.setPosition(450, 360);
    window.draw(subtitleText);

    sf::Uint8 pa = static_cast<sf::Uint8>(150 + 105 * std::abs(std::sin(titlePulse * 2.5f)));
    sf::Text prompt;
    prompt.setFont(font);
    prompt.setCharacterSize(22);
    prompt.setFillColor(sf::Color(255, 220, 50, pa));
    prompt.setString("PRESS SPACE to Play Again  |  ESC to Exit");
    sf::FloatRect pb = prompt.getLocalBounds();
    prompt.setOrigin(pb.width / 2, 0);
    prompt.setPosition(450, 435);
    window.draw(prompt);
}

void Game::renderWinScreen() {
    sf::RectangleShape overlay(sf::Vector2f(900, 700));
    overlay.setFillColor(sf::Color(0, 0, 0, 140));
    window.draw(overlay);

    sf::RectangleShape panel(sf::Vector2f(660, 350));
    panel.setPosition(120, 175);
    panel.setFillColor(sf::Color(10, 10, 50, 220));
    float t = std::abs(std::sin(titlePulse * 1.5f));
    panel.setOutlineColor(sf::Color(
        static_cast<sf::Uint8>(100 + 155*t),
        static_cast<sf::Uint8>(200),
        255));
    panel.setOutlineThickness(3.0f);
    window.draw(panel);

    gameStateText.setCharacterSize(56);
    gameStateText.setStyle(sf::Text::Bold);
    gameStateText.setFillColor(sf::Color(255, 220, 50));
    gameStateText.setString("YOU WIN!");
    sf::FloatRect lb = gameStateText.getLocalBounds();
    gameStateText.setOrigin(lb.width / 2, lb.height / 2);
    gameStateText.setPosition(450, 265);
    window.draw(gameStateText);

    subtitleText.setCharacterSize(22);
    subtitleText.setFillColor(sf::Color(200, 230, 255));
    subtitleText.setString("All levels conquered!\nFinal Score: " + std::to_string(score));
    sf::FloatRect slb = subtitleText.getLocalBounds();
    subtitleText.setOrigin(slb.width / 2, slb.height / 2);
    subtitleText.setPosition(450, 360);
    window.draw(subtitleText);

    sf::Uint8 pa = static_cast<sf::Uint8>(150 + 105 * std::abs(std::sin(titlePulse * 2.5f)));
    sf::Text prompt;
    prompt.setFont(font);
    prompt.setCharacterSize(22);
    prompt.setFillColor(sf::Color(255, 220, 50, pa));
    prompt.setString("PRESS SPACE to Play Again");
    sf::FloatRect pb = prompt.getLocalBounds();
    prompt.setOrigin(pb.width / 2, 0);
    prompt.setPosition(450, 445);
    window.draw(prompt);
}

// ============================================================
// SPAWN ENEMY
// ============================================================
void Game::spawnEnemy() {
    float x = static_cast<float>(rand() % 820 + 40);
    float y = -60.0f;

    EnemyType type;
    int randType = rand() % 100;

    switch (currentLevel) {
        case Level::LEVEL_1:
            type = EnemyType::BASIC;
            break;
        case Level::LEVEL_2:
            type = (randType < 55) ? EnemyType::BASIC : EnemyType::FAST;
            break;
        case Level::LEVEL_3:
            if (randType < 35)      type = EnemyType::BASIC;
            else if (randType < 75) type = EnemyType::FAST;
            else                    type = EnemyType::HEAVY;
            break;
        default:
            type = EnemyType::BASIC;
            break;
    }

    Enemy enemy(type, x, y);

    // Assign correct image PER ENEMY TYPE (not per level)
    switch (type) {
        case EnemyType::BASIC:
            enemy.setTexture("assets/enemy_alien.png");  // Green alien saucer
            break;
        case EnemyType::FAST:
            enemy.setTexture("assets/enemy_robot.png");  // Purple robot fighter
            break;
        case EnemyType::HEAVY:
            enemy.setTexture("assets/enemy_demon.png");  // Demon battleship
            break;
    }

    const auto& config = levelConfigs[currentLevel];
    enemy.setSpeed(config.enemySpeed * difficultyMultiplier);
    enemies.push_back(enemy);
}

// ============================================================
// HANDLE COLLISIONS
// ============================================================
void Game::handleCollisions() {
    // Player - Enemy body collision
    for (auto& enemy : enemies) {
        if (Collision::checkSpriteCollision(player.getSprite(), enemy.getSprite())) {
            player.takeDamage(25);
            enemy.takeDamage(enemy.getHealth());
            triggerScreenShake(8.0f, 0.35f);
            sf::FloatRect eb = enemy.getBounds();
            particles.spawnExplosion(eb.left + eb.width/2, eb.top + eb.height/2,
                                      sf::Color(255, 140, 30), 14);
        }
    }

    // Player bullet - Enemy collision
    for (auto& bullet : bullets) {
        if (bullet.getType() == BulletType::PLAYER) {
            for (auto& enemy : enemies) {
                if (enemy.isAlive() && Collision::checkBulletSpriteCollision(bullet, enemy.getSprite())) {
                    enemy.takeDamage(bullet.getDamage());
                    bullet.setActive(false);

                    // Hit flash particle
                    sf::FloatRect bb = bullet.getBounds();
                    particles.spawnExplosion(bb.left, bb.top, sf::Color(255, 255, 100), 5);

                    if (!enemy.isAlive()) {
                        score += enemy.getScoreValue();
                        sf::FloatRect eb = enemy.getBounds();
                        float ex = eb.left + eb.width/2;
                        float ey = eb.top + eb.height/2;
                        // Big explosion
                        particles.spawnExplosion(ex, ey, sf::Color(255, 80, 20), 22);
                        particles.spawnExplosion(ex, ey, sf::Color(255, 200, 50), 12);
                        particles.spawnPowerUpGlow(ex, ey, sf::Color(255, 180, 30));
                        triggerScreenShake(3.0f, 0.15f);
                        soundManager.playSound(SoundType::EXPLOSION);
                        spawnPowerUp(ex, ey);
                    }
                }
            }
        }
    }

    // Enemy bullet - Player collision
    for (auto& bullet : bullets) {
        if (bullet.getType() == BulletType::ENEMY) {
            if (Collision::checkBulletSpriteCollision(bullet, player.getSprite())) {
                player.takeDamage(bullet.getDamage());
                bullet.setActive(false);
                triggerScreenShake(5.0f, 0.2f);
                soundManager.playSound(SoundType::SHIELD_HIT);
                sf::FloatRect pb = player.getBounds();
                particles.spawnExplosion(pb.left + pb.width/2, pb.top,
                                          sf::Color(100, 150, 255), 8);
            }
        }
    }

    handlePowerUpCollisions();
}

// ============================================================
// SCREEN SHAKE
// ============================================================
void Game::triggerScreenShake(float magnitude, float duration) {
    shakeMagnitude = magnitude;
    shakeTimer = duration;
}

void Game::updateScreenShake(float dt) {
    if (shakeTimer > 0.0f) {
        shakeTimer -= dt;
        float intensity = shakeMagnitude * (shakeTimer / 0.5f);
        intensity = std::min(intensity, shakeMagnitude);
        cameraOffset.x = (rand() % 100 / 50.0f - 1.0f) * intensity;
        cameraOffset.y = (rand() % 100 / 50.0f - 1.0f) * intensity;
    } else {
        shakeTimer = 0.0f;
        cameraOffset = sf::Vector2f(0, 0);
    }
}

// ============================================================
// UI
// ============================================================
void Game::updateUI() {
    scoreText.setString("SCORE: " + std::to_string(score));
    levelText.setString("LVL " + std::to_string(static_cast<int>(currentLevel) + 1));
    targetText.setString("TARGET: " + std::to_string(targetScore));
}

void Game::setupUI() {
    // Load font
    bool fontLoaded = false;
    const char* fonts[] = {
        "C:/Windows/Fonts/consola.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf"
    };
    for (const char* f : fonts) {
        if (font.loadFromFile(f)) { fontLoaded = true; break; }
    }
    if (!fontLoaded) {
        std::cout << "Warning: Could not load font." << std::endl;
    }

    // HUD panel at top
    hudPanel.setSize(sf::Vector2f(900, 55));
    hudPanel.setPosition(0, 0);
    hudPanel.setFillColor(sf::Color(5, 10, 30, 210));
    hudPanel.setOutlineColor(sf::Color(40, 100, 200, 180));
    hudPanel.setOutlineThickness(1.5f);

    // Score
    scoreText.setFont(font);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(sf::Color(100, 220, 255));
    scoreText.setStyle(sf::Text::Bold);
    scoreText.setPosition(14, 16);

    // Level
    levelText.setFont(font);
    levelText.setCharacterSize(20);
    levelText.setFillColor(sf::Color(255, 200, 50));
    levelText.setStyle(sf::Text::Bold);
    levelText.setPosition(260, 16);

    // Target
    targetText.setFont(font);
    targetText.setCharacterSize(20);
    targetText.setFillColor(sf::Color(200, 160, 255));
    targetText.setPosition(380, 16);

    // Health label above health bar
    healthLabel.setFont(font);
    healthLabel.setCharacterSize(15);
    healthLabel.setFillColor(sf::Color(200, 255, 200));
    healthLabel.setPosition(690, 40);

    // Power-up status
    powerUpStatus.setFont(font);
    powerUpStatus.setCharacterSize(18);
    powerUpStatus.setStyle(sf::Text::Bold);
    powerUpStatus.setPosition(14, 660);

    // FPS counter
    fpsText.setFont(font);
    fpsText.setCharacterSize(14);
    fpsText.setFillColor(sf::Color(120, 120, 120));
    fpsText.setPosition(840, 2);

    // General state text
    gameStateText.setFont(font);
    gameStateText.setCharacterSize(50);
    gameStateText.setStyle(sf::Text::Bold);
    gameStateText.setFillColor(sf::Color::White);

    subtitleText.setFont(font);
    subtitleText.setCharacterSize(24);
    subtitleText.setFillColor(sf::Color(200, 200, 200));

    controlsText.setFont(font);
    controlsText.setCharacterSize(18);

    // Nebula layer (transparent colored overlay per level)
    nebulaLayer.setSize(sf::Vector2f(900, 700));
    nebulaLayer.setPosition(0, 0);
    nebulaLayer.setFillColor(sf::Color(20, 10, 40, 30)); // default purple tint
}

void Game::checkForCustomImages() {
    std::cout << "Checking for custom images..." << std::endl;
}

// ============================================================
// LEVEL MANAGEMENT
// ============================================================
void Game::initializeLevels() {
    // Use the transparent-background PNG assets for vivid enemy visuals
    levelConfigs[Level::LEVEL_1] = {
        300, 5, 1.8f, 90.0f,
        "assets/player_ship.png",
        "assets/enemy_alien.png",   // Green alien saucer
        "", ""
    };
    levelConfigs[Level::LEVEL_2] = {
        700, 8, 1.3f, 135.0f,
        "assets/player_ship.png",
        "assets/enemy_robot.png",   // Purple robot warrior
        "", ""
    };
    levelConfigs[Level::LEVEL_3] = {
        1200, 12, 0.9f, 180.0f,
        "assets/player_ship.png",
        "assets/enemy_demon.png",   // Demon battleship
        "", ""
    };
}

void Game::setupLevel(Level level) {
    currentLevel = level;
    const auto& config = levelConfigs[level];

    targetScore = config.targetScore;
    enemySpawnInterval = config.enemySpawnRate;
    difficultyMultiplier = 1.0f + (static_cast<int>(level) * 0.4f);

    // Player health scaling
    switch (level) {
        case Level::LEVEL_1:
            player.setMaxHealth(100);
            player.heal(100);
            nebulaLayer.setFillColor(sf::Color(10, 5, 30, 28));
            break;
        case Level::LEVEL_2:
            player.setMaxHealth(200);
            player.heal(200);
            nebulaLayer.setFillColor(sf::Color(5, 25, 10, 28));
            break;
        case Level::LEVEL_3:
            player.setMaxHealth(500);
            player.heal(500);
            nebulaLayer.setFillColor(sf::Color(30, 5, 5, 35));
            break;
    }

    enemies.clear();
    bullets.clear();
    powerUps.clear();
    hasRapidFire = false;
    rapidFireTimer = 0.0f;

    if (!config.playerImage.empty()) {
        player.setTexture(config.playerImage);
    }

    // Staggered enemy spawn positions
    for (int i = 0; i < config.enemyCount; ++i) {
        spawnEnemy();
    }

    updateUI();
    std::cout << "Level " << (static_cast<int>(level)+1) << " started. Target: " << targetScore << std::endl;
}

void Game::nextLevel() {
    score = 0;
    switch (currentLevel) {
        case Level::LEVEL_1: setupLevel(Level::LEVEL_2); break;
        case Level::LEVEL_2: setupLevel(Level::LEVEL_3); break;
        case Level::LEVEL_3:
            currentState = GameState::WIN;
            break;
    }
}

void Game::checkLevelComplete() {
    if (score >= targetScore && currentState == GameState::PLAYING) {
        currentState = GameState::LEVEL_COMPLETE;
        // Celebration particles
        for (int i = 0; i < 5; ++i) {
            float px = static_cast<float>(rand() % 800 + 50);
            float py = static_cast<float>(rand() % 400 + 100);
            particles.spawnExplosion(px, py, sf::Color(255, 220, 50), 15);
        }
    }
}

void Game::setLevel(Level level) { setupLevel(level); }
Level Game::getCurrentLevel() const { return currentLevel; }
int Game::getTargetScore() const { return targetScore; }
bool Game::isLevelComplete() const { return score >= targetScore; }

// ============================================================
// POWER-UPS
// ============================================================
void Game::spawnPowerUp(float x, float y) {
    if (static_cast<float>(rand()) / RAND_MAX < powerUpSpawnChance) {
        powerUps.emplace_back(x, y);
    }
}

void Game::updatePowerUps(float deltaTime) {
    if (hasRapidFire) {
        rapidFireTimer -= deltaTime;
        if (rapidFireTimer <= 0) {
            hasRapidFire = false;
            player.setFireRate(0.18f);
            std::cout << "Rapid fire ended." << std::endl;
        }
    }
    for (auto& pu : powerUps) {
        pu.update(deltaTime);
        // Spawn ambient glow particles
        if (rand() % 20 == 0) {
            sf::Vector2f pos = pu.getPosition();
            sf::Color glowColor = (pu.getType() == PowerUpType::RAPID_FIRE)
                ? sf::Color(255, 230, 30) : sf::Color(50, 255, 100);
            particles.spawnPowerUpGlow(pos.x, pos.y, glowColor);
        }
    }
    powerUps.erase(
        std::remove_if(powerUps.begin(), powerUps.end(),
            [](PowerUp& pu) {
                return !pu.isActive() || pu.isExpired() || pu.isOutOfBounds(900, 700);
            }),
        powerUps.end()
    );
}

void Game::handlePowerUpCollisions() {
    for (auto& pu : powerUps) {
        if (pu.isActive() && Collision::checkSpriteCollision(player.getSprite(), pu.getSprite())) {
            applyPowerUpEffect(pu.getType());
            // Collect burst
            sf::Vector2f pos = pu.getPosition();
            particles.spawnExplosion(pos.x, pos.y,
                (pu.getType() == PowerUpType::RAPID_FIRE)
                    ? sf::Color(255, 230, 30) : sf::Color(50, 255, 100), 18);
            pu.setActive(false);
            soundManager.playSound(SoundType::POWERUP_PICKUP);
            std::cout << "Collected: " << pu.getTypeName() << std::endl;
        }
    }
}

void Game::applyPowerUpEffect(PowerUpType type) {
    switch (type) {
        case PowerUpType::RAPID_FIRE:
            hasRapidFire = true;
            rapidFireTimer = 5.0f;
            player.setFireRate(0.04f);
            std::cout << "RAPID FIRE activated! 5 seconds." << std::endl;
            break;
        case PowerUpType::HEALTH: {
            int heal = static_cast<int>(player.getMaxHealth() * 0.2f);
            player.heal(heal);
            std::cout << "Health Boost! +" << heal << " HP" << std::endl;
            break;
        }
        default: break;
    }
}
