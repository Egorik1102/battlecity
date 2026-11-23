#include "Game.h"
#include "raylib.h"

void Game::Init() {}

void Game::Update(float dt) {}

void Game::Draw() {
    ClearBackground({ 30, 30, 35, 255 });
    DrawText("Battlecity Skeleton Works!", 20, 20, 30, LIGHTGRAY);
    DrawText("WASD to move (soon)", 20, 60, 20, GRAY);
}

void Game::Run() {
    Init();
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        Update(dt);
        BeginDrawing();
        Draw();
        EndDrawing();
    }
}
