#pragma once
#include "Tank.h"
#include "Bullet.h"
#include "Map.h"
#include "Menu.h"
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
    GameState state = GameState::Menu;   // Игра стартует из главного меню
    Tank* player = nullptr;
    Map* map = nullptr;
    Menu* menu = nullptr;
    std::vector<Bullet> bullets;

    int screenWidth = 1280;
    int screenHeight = 720;

    // Перезапуск уровня: создаёт заново игрока, карту, чистит пули.
    void Restart();
};
