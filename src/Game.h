#pragma once
#include "Tank.h"
#include "Bullet.h"
#include "Map.h"
#include <vector>

enum class GameState { Menu, Playing, Paused, GameOver };

class Game {
public:
    void Run();
    void Init();
    void Update(float dt);
    void Draw();
private:
    GameState state = GameState::Playing; 
    Tank* player = nullptr;
    Map* map = nullptr;
    std::vector<Bullet> bullets;
};
