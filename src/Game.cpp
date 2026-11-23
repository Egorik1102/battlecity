#include "Game.h"
#include "raylib.h"
#include <algorithm>

void Game::Init() {
    player = new Tank({ 640, 360 }); 
}

void Game::Update(float dt) {
    if (state == GameState::Playing) {
        if (player) player->Update(dt);

        // Стрельба по Пробелу
        if (IsKeyPressed(KEY_SPACE) && bullets.size() < 5) {
            bullets.push_back(Bullet(player->pos, player->bodyAngle));
        }

        // Обновление пуль
        for (auto& b : bullets) b.Update(dt);
        
        // Удаление неактивных пуль
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(), 
            [](const Bullet& b) { return !b.active; }), bullets.end());
    }
}

void Game::Draw() {
    ClearBackground({ 30, 30, 35, 255 });
    if (state == GameState::Playing) {
        if (player) player->Draw();
        for (auto& b : bullets) b.Draw();
    }
    DrawText("WASD - move | Space - shoot", 20, 20, 20, LIGHTGRAY);
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
