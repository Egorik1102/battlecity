#pragma once
#include "Tank.h"
#include "Bullet.h"
#include "Map.h"
#include "Menu.h"
#include "Enemy.h"
#include <vector>

enum class GameState { Menu, Playing, Paused, GameOver };

class Game {
public:
    Game();
    void Run();
    void Init();
    void Update(float dt);
    void Draw();
private:
    GameState state = GameState::Menu;
    Tank* player = nullptr;
    Map* map = nullptr;
    Menu* menu = nullptr;
    std::vector<Bullet> bullets;
    std::vector<Bullet> enemyBullets;
    std::vector<Enemy> enemies;
    int screenWidth = 1280;
    int screenHeight = 720;
    void Restart();

    // --- Волны ---
    int enemiesToSpawn = 20;
    float spawnCooldown = 2.0f;
    std::vector<Vector2> spawnPoints;   // <-- изменено
    void SpawnEnemy();
};