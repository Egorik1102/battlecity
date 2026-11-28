#include "Game.h"
#include "raylib.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

Game::Game() {
    menu = new Menu(screenWidth, screenHeight);
}

void Game::Init() {
    map = new Map();
    map->Init();

    // Игрок появляется внизу-по-центру
    Vector2 startPos = {
        map->offset.x + 15 * Map::TILE_SIZE + Map::TILE_SIZE / 2.0f,
        map->offset.y + 15 * Map::TILE_SIZE + Map::TILE_SIZE / 2.0f
    };
    player = new Tank(startPos);

    // --- 6 точек спавна врагов (верхняя часть карты) ---
    // Расставляем их вдоль верхней стены, чтобы не было скученности
    spawnPoints = {
        { map->offset.x + 1  * Map::TILE_SIZE + Map::TILE_SIZE / 2.0f, map->offset.y + 1 * Map::TILE_SIZE + Map::TILE_SIZE / 2.0f },
        { map->offset.x + 7  * Map::TILE_SIZE + Map::TILE_SIZE / 2.0f, map->offset.y + 1 * Map::TILE_SIZE + Map::TILE_SIZE / 2.0f },
        { map->offset.x + 15 * Map::TILE_SIZE + Map::TILE_SIZE / 2.0f, map->offset.y + 1 * Map::TILE_SIZE + Map::TILE_SIZE / 2.0f },
        { map->offset.x + 23 * Map::TILE_SIZE + Map::TILE_SIZE / 2.0f, map->offset.y + 1 * Map::TILE_SIZE + Map::TILE_SIZE / 2.0f },
        { map->offset.x + 3  * Map::TILE_SIZE + Map::TILE_SIZE / 2.0f, map->offset.y + 3 * Map::TILE_SIZE + Map::TILE_SIZE / 2.0f },
        { map->offset.x + 28 * Map::TILE_SIZE + Map::TILE_SIZE / 2.0f, map->offset.y + 3 * Map::TILE_SIZE + Map::TILE_SIZE / 2.0f }
    };

    enemies.clear();
    enemiesToSpawn = 20;
}

void Game::SpawnEnemy() {
    // Перемешиваем точки спавна, чтобы враги появлялись в случайном порядке
    // (но проверяем их по очереди)
    int startIdx = rand() % (int)spawnPoints.size();
    
    for (int i = 0; i < (int)spawnPoints.size(); ++i) {
        int idx = (startIdx + i) % spawnPoints.size();
        Vector2 sp = spawnPoints[idx];
        
        // Создаем временный прямоугольник для проверки
        Rectangle spawnRect = { sp.x - 20, sp.y - 20, 40, 40 };
        
        // 1. Проверка на стену
        if (map->CheckRectCollision(spawnRect)) continue;
        
        // 2. Проверка на игрока
        if (player && CheckCollisionRecs(spawnRect, player->GetRect())) continue;
        
        // 3. Проверка на других врагов
        bool occupied = false;
        for (const auto& e : enemies) {
            if (e.active && CheckCollisionRecs(spawnRect, e.GetRect())) {
                occupied = true;
                break;
            }
        }
        if (occupied) continue;

        // Если все проверки пройдены — спавним!
        enemies.push_back(Enemy(sp));
        return; 
    }
    // Если все точки заняты — враг не спавнится в этом кадре, подождем следующий
}

void Game::Restart() {
    delete player; delete map;
    player = nullptr; map = nullptr;
    bullets.clear(); enemyBullets.clear(); enemies.clear();
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

        // Логика волн
        if (enemiesToSpawn > 0) {
            int activeEnemies = 0;
            for (const auto& e : enemies) {
                if (e.active) activeEnemies++;
            }
            if (activeEnemies < 4) {
                SpawnEnemy();
                enemiesToSpawn--;
            }
        }

        if (player) {
            Vector2 oldPos = player->pos;
            player->Update(dt);

            if (map->CheckRectCollision(player->GetRect())) {
                player->pos = oldPos;
            }

            for (auto& e : enemies) {
                if (e.active && CheckCollisionRecs(player->GetRect(), e.GetRect())) {
                    player->pos = oldPos;
                    break;
                }
            }

            float minX = map->offset.x + 20.0f;
            float maxX = map->offset.x + map->GetWidthPx() - 20.0f;
            float minY = map->offset.y + 20.0f;
            float maxY = map->offset.y + map->GetHeightPx() - 20.0f;
            if (player->pos.x < minX) player->pos.x = minX;
            if (player->pos.x > maxX) player->pos.x = maxX;
            if (player->pos.y < minY) player->pos.y = minY;
            if (player->pos.y > maxY) player->pos.y = maxY;
        }

        if (IsKeyPressed(KEY_SPACE) && bullets.size() < 5) {
            bullets.push_back(Bullet(player->pos, player->bodyAngle));
        }
        for (auto& b : bullets) {
            b.Update(dt);
            if (b.active && map->IsSolidAt(b.pos.x, b.pos.y)) {
                b.active = false;
            }
        }
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b) { return !b.active; }), bullets.end());

        for (auto& e : enemies) {
            if (e.active) {
                Vector2 oldEnemyPos = e.pos;
                e.Update(player->pos, dt);

                if (map->CheckRectCollision(e.GetRect())) {
                    e.pos = oldEnemyPos;
                    e.moveTimer = 0.0f;
                }

                if (player && CheckCollisionRecs(e.GetRect(), player->GetRect())) {
                    e.pos = oldEnemyPos;
                    e.moveTimer = 0.0f;
                }

                for (auto& other : enemies) {
                    if (&other == &e || !other.active) continue;
                    if (CheckCollisionRecs(e.GetRect(), other.GetRect())) {
                        e.pos = oldEnemyPos;
                        e.moveTimer = 0.0f;
                        break;
                    }
                }

                if (e.shootCooldown <= 0.0f) {
                    enemyBullets.push_back(Bullet(e.pos, e.bodyAngle));
                    e.shootCooldown = 2.0f;
                }

                for (auto& b : bullets) {
                    if (b.active && CheckCollisionRecs(e.GetRect(), {b.pos.x, b.pos.y, 8, 8})) {
                        e.TakeDamage(1);
                        b.active = false;
                    }
                }
            }
        }

        for (auto& b : enemyBullets) {
            b.Update(dt);
            if (b.active) {
                if (map->IsSolidAt(b.pos.x, b.pos.y)) {
                    b.active = false;
                } else if (CheckCollisionRecs({b.pos.x - 4, b.pos.y - 4, 8, 8}, player->GetRect())) {
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
    if (state == GameState::Menu) { menu->Draw(); return; }

    if (map) map->Draw();
    if (player) player->Draw();
    for (auto& b : bullets) b.Draw();
    for (auto& b : enemyBullets) b.Draw();
    for (auto& e : enemies) e.Draw();

    if (player) {
        DrawText(TextFormat("HP: %d", player->hp), 20, screenHeight - 40, 30, WHITE);
        DrawText("SPACE - shoot", screenWidth - 200, screenHeight - 40, 20, LIGHTGRAY);
        DrawText(TextFormat("Enemies left: %d", enemiesToSpawn), 20, 20, 20, LIGHTGRAY);
    }

    if (state == GameState::Paused) {
        DrawRectangle(0, 0, screenWidth, screenHeight, { 0, 0, 0, 160 });
        DrawText("PAUSE", screenWidth / 2 - 50, screenHeight / 2, 40, WHITE);
    }
    if (state == GameState::GameOver) {
        DrawRectangle(0, 0, screenWidth, screenHeight, { 0, 0, 0, 200 });
        DrawText("GAME OVER", screenWidth / 2 - 100, screenHeight / 2 - 20, 40, RED);
        DrawText("Press ENTER to return to Menu", screenWidth / 2 - 150, screenHeight / 2 + 40, 20, WHITE);
    }
}

void Game::Run() {
    srand((unsigned)time(nullptr));
    Init();
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        Update(dt);
        BeginDrawing(); Draw(); EndDrawing();
    }
    delete player; delete map; delete menu;
}
