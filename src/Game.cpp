#include "Game.h"
#include "raylib.h"
#include <algorithm>
#include <cmath>

Game::Game() {
    menu = new Menu(screenWidth, screenHeight);
}

void Game::Init() {
    player = new Tank({ 640, 360 });
    map = new Map();
    map->Init();
    
    // Спавним 3 врага
    enemies.clear();
    enemies.push_back(Enemy({ 100, 100 }));
    enemies.push_back(Enemy({ 1180, 100 }));
    enemies.push_back(Enemy({ 640, 100 }));
}

void Game::Restart() {
    delete player;
    delete map;
    player = nullptr;
    map = nullptr;
    bullets.clear();
    enemies.clear();
    Init();
}

void Game::Update(float dt) {
    if (state == GameState::Menu) {
        MenuAction action = menu->Update(dt);
        if (action == MenuAction::Play) { Restart(); state = GameState::Playing; }
        if (action == MenuAction::Exit) { CloseWindow(); }
        return;
    }

    if (state == GameState::Paused) {
        if (IsKeyPressed(KEY_ESCAPE)) state = GameState::Playing;
        if (IsKeyPressed(KEY_ENTER)) { state = GameState::Menu; menu->Reset(); }
        return;
    }

    if (state == GameState::GameOver) {
        if (IsKeyPressed(KEY_ENTER)) { state = GameState::Menu; menu->Reset(); }
        return;
    }

    if (state == GameState::Playing) {
        if (IsKeyPressed(KEY_ESCAPE)) { state = GameState::Paused; return; }

        if (player) {
            Vector2 oldPos = player->pos;
            player->Update(dt);
	    // Ограничение по экрану
	    if (player->pos.x < 20) player->pos.x = 20;
	    if (player->pos.x > 1260) player->pos.x = 1260;
	    if (player->pos.y < 20) player->pos.y = 20;
	    if (player->pos.y > 700) player->pos.y = 700;
            for (const auto& wall : map->walls) {
                if (CheckCollisionRecs(player->GetRect(), wall)) {
                    player->pos = oldPos;
                    break;
                }
            }
        }

        // Стрельба
        if (IsKeyPressed(KEY_SPACE) && bullets.size() < 5) {
            bullets.push_back(Bullet(player->pos, player->bodyAngle));
        }

        // Обновление пуль и проверка столкновения со стенами
for (auto& b : bullets) {
    b.Update(dt);
    if (b.active) {
        for (const auto& wall : map->walls) {
            if (CheckCollisionRecs({b.pos.x - 4, b.pos.y - 4, 8, 8}, wall)) {
                b.active = false;
                break;
            }
        }
    }
}
        // Обновление пуль
        for (auto& b : bullets) b.Update(dt);
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(), 
            [](const Bullet& b) { return !b.active; }), bullets.end());

        // Обновление врагов и проверка урона
        for (auto& e : enemies) {
            if (e.active) {
                e.Update(player->pos, dt);
		Vector2 oldEnemyPos = e.pos; // (нужно добавить поле oldPos в Enemy, или просто сохранить)
        for (const auto& wall : map->walls) {
            if (CheckCollisionRecs(e.GetRect(), wall)) {
                // Откатываем позицию врага, если врезался
                // (Для этого в Enemy.cpp в методе Update нужно сохранить старую позицию и откатывать её)
            }
        }
                
                // Враг бьет игрока (если близко)
                if (CheckCollisionRecs(player->GetRect(), e.GetRect())) {
                    player->TakeDamage(10);
                }

                // Пули бьют врага
                for (auto& b : bullets) {
                    if (b.active && CheckCollisionRecs(e.GetRect(), {b.pos.x, b.pos.y, 8, 8})) {
                        e.TakeDamage(10);
                        b.active = false;
                    }
                }
            }
        }

        // Проверка смерти
        if (player->hp <= 0) state = GameState::GameOver;
    }
}

void Game::Draw() {
    ClearBackground({ 30, 30, 35, 255 });

    if (state == GameState::Menu) {
        menu->Draw();
        return;
    }

    if (map) map->Draw();
    if (player) player->Draw();
    for (auto& b : bullets) b.Draw();
    for (auto& e : enemies) e.Draw();

    if (state == GameState::Paused) {
        DrawRectangle(0, 0, screenWidth, screenHeight, { 0, 0, 0, 160 });
        DrawText("PAUSE", screenWidth/2 - 50, screenHeight/2, 40, WHITE);
    }

    if (state == GameState::GameOver) {
        DrawRectangle(0, 0, screenWidth, screenHeight, { 0, 0, 0, 200 });
        DrawText("GAME OVER", screenWidth/2 - 100, screenHeight/2 - 20, 40, RED);
        DrawText("Press ENTER to return to Menu", screenWidth/2 - 150, screenHeight/2 + 40, 20, WHITE);
    }
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
    delete map;
    delete menu;
}
