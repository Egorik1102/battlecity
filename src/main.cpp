#include "raylib.h"
#include "Game.h"

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Battlecity - 2D Tanks");
    SetTargetFPS(60);

    Game game;
    game.Run();

    CloseWindow();
    return 0;
}
