#include "Game.h"
#include "raylib.h"

void Game::Init() {
    player = new Tank({ 640, 360 }); 
}

void Game::Update(float dt) {
    if (state == GameState::Playing) {
        if (player) player->Update(dt);
    }
}

void Game::Draw() {
    ClearBackground({ 30, 30, 35, 255 });
    if (state == GameState::Playing) {
        if (player) player->Draw();
    }
    DrawText("Battlecity - WASD to move", 20, 20, 20, LIGHTGRAY);
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
    delete player;
}
