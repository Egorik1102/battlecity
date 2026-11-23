#pragma once
#include "Tank.h"

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
};
