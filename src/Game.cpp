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
    enemyBullets.clear();
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

        if (IsKeyPressed(KEY_SPACE) && bullets.size() < 5) {
            bullets.push_back(Bullet(player->pos, player->bodyAngle));
        }

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
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(), 
            [](const Bullet& b) { return !b.active; }), bullets.end());

        for (auto& e : enemies) {
            if (e.active) {
                Vector2 oldEnemyPos = e.pos;
                e.Update(player->pos, dt);
                
                for (const auto& wall : map->walls) {
                    if (CheckCollisionRecs(e.GetRect(), wall)) {
                        e.pos = oldEnemyPos;
                        e.moveTimer = 0.0f;
                        break;
                    }
                }

                if (e.shootCooldown <= 0.0f) {
                    enemyBullets.push_back(Bullet(e.pos, e.bodyAngle));
                    e.shootCooldown = 2.0f;
                }

                if (CheckCollisionRecs(player->GetRect(), e.GetRect())) {
                    player->TakeDamage(10);
                }

                for (auto& b : bullets) {
                    if (b.active && CheckCollisionRecs(e.GetRect(), {b.pos.x, b.pos.y, 8, 8})) {
                        e.TakeDamage(10);
                        b.active = false;
                    }
                }
            }
        }

        for (auto& b : enemyBullets) {
            b.Update(dt);
            if (b.active) {
                for (const auto& wall : map->walls) {
                    if (CheckCollisionRecs({b.pos.x - 4, b.pos.y - 4, 8, 8}, wall)) {
                        b.active = false;
                        break;
                    }
                }
                if (CheckCollisionRecs({b.pos.x - 4, b.pos.y - 4, 8, 8}, player->GetRect())) {
                    player->TakeDamage(10);
                    b.active = false;
                }
            }
        }
        enemyBullets.erase(std::remove_if(enemyBullets.begin(), enemyBullets.end(), 
            [](const Bullet& b) { return !b.active; }), enemyBullets.end());

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
    for (auto& b : enemyBullets) b.Draw();
    for (auto& e : enemies) e.Draw();

    if (player) {
        DrawText(TextFormat("HP: %d", player->hp), 20, screenHeight - 40, 30, WHITE);
        DrawText("SPACE - shoot", screenWidth - 200, screenHeight - 40, 20, LIGHTGRAY);
    }

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
