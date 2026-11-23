#include "raylib.h"
#include "Game.h"

int main() {
    InitWindow(1280, 720, "Battlecity - 2D Tanks");
    SetTargetFPS(60);
    Game game;
    game.Run();
    CloseWindow();
    return 0;
}
