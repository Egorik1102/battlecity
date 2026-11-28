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

    // Игрок появляется внизу-по-центру (как в оригинале)
    Vector2 startPos = {
        map->offset.x + 15 * Map::TILE_SIZE + Map::TILE_SIZE / 2.0f,
        map->offset.y + 15 * Map::TILE_SIZE + Map::TILE_SIZE / 2.0f
    };
    player = new Tank(startPos);

    // --- Точки спавна врагов (верхняя часть карты) ---
    spawnPoints = {
        { map->offset.x + 1  * Map::TILE_SIZE + Map::TILE_SIZE / 2.0f, map->offset.y + 1 * Map::TILE_SIZE + Map::TILE_SIZE / 2.0f },
        { map->offset.x + 13 * Map::TILE_SIZE + Map::TILE_SIZE / 2.0f, map->offset.y + 1 * Map::TILE_SIZE + Map::TILE_SIZE / 2.0f },
        { map->offset.x + 7  * Map::TILE_SIZE + Map::TILE_SIZE / 2.0f, map->offset.y + 1 * Map::TILE_SIZE + Map::TILE_SIZE / 2.0f },
    };

    // --- Волны ---
    enemies.clear();
    enemiesToSpawn = 20;
    spawnCooldown = 0.0f;

    SpawnEnemy();          // первый враг сразу
    enemiesToSpawn--;      // учитываем его в счётчике
    spawnCooldown = 2.0f;  // следующий через 2 секунды
}

void Game::SpawnEnemy() {
    int idx = rand() % (int)spawnPoints.size();
    enemies.push_back(Enemy(spawnPoints[idx]));
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

        // ================= ЛОГИКА ВОЛН =================
        if (enemiesToSpawn > 0) {
            spawnCooldown -= dt;

            int activeEnemies = 0;
            for (const auto& e : enemies)
                if (e.active) activeEnemies++;

            if (activeEnemies < 4 && spawnCooldown <= 0.0f) {
                SpawnEnemy();
                enemiesToSpawn--;
                spawnCooldown = 2.0f; // следующий через 2 секунды
            }
        }

        // ================= 1. ИГРОК И КОЛЛИЗИИ =================
        if (player) {
            Vector2 oldPos = player->pos;
            player->Update(dt);

            // Стены
            if (map->CheckRectCollision(player->GetRect())) {
                player->pos = oldPos;
            }

            // Игрок врезался во врага — откат (без урона)
            for (auto& e : enemies) {
                if (e.active && CheckCollisionRecs(player->GetRect(), e.GetRect())) {
                    player->pos = oldPos;
                    break;
                }
            }

            // Ограничение по краям карты
            float minX = map->offset.x + 20.0f;
            float maxX = map->offset.x + map->GetWidthPx() - 20.0f;
            float minY = map->offset.y + 20.0f;
            float maxY = map->offset.y + map->GetHeightPx() - 20.0f;
            if (player->pos.x < minX) player->pos.x = minX;
            if (player->pos.x > maxX) player->pos.x = maxX;
            if (player->pos.y < minY) player->pos.y = minY;
            if (player->pos.y > maxY) player->pos.y = maxY;
        }

        // ================= 2. ПУЛИ ИГРОКА =================
        if (IsKeyPressed(KEY_SPACE) && bullets.size() < 5) {
            bullets.push_back(Bullet(player->pos, player->bodyAngle));
        }
        for (auto& b : bullets) {
            b.Update(dt);
            if (b.active && map->IsSolidAt(b.pos.x, b.pos.y)) {
                b.active = false; // Пуля врезалась в стену
            }
        }
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b) { return !b.active; }), bullets.end());

        // ================= 3. ВРАГИ =================
        for (auto& e : enemies) {
            if (e.active) {
                Vector2 oldEnemyPos = e.pos;
                e.Update(player->pos, dt);

                // Стены
                if (map->CheckRectCollision(e.GetRect())) {
                    e.pos = oldEnemyPos;
                    e.moveTimer = 0.0f;
                }

                // Враг врезался в игрока — откат (без урона)
                if (player && CheckCollisionRecs(e.GetRect(), player->GetRect())) {
                    e.pos = oldEnemyPos;
                    e.moveTimer = 0.0f;
                }

                // Враг врезался в другого врага — откат
                for (auto& other : enemies) {
                    if (&other == &e || !other.active) continue;
                    if (CheckCollisionRecs(e.GetRect(), other.GetRect())) {
                        e.pos = oldEnemyPos;
                        e.moveTimer = 0.0f;
                        break;
                    }
                }

                // Стрельба врага
                if (e.shootCooldown <= 0.0f) {
                    enemyBullets.push_back(Bullet(e.pos, e.bodyAngle));
                    e.shootCooldown = 2.0f;
                }

                // Попадание пуль игрока по врагу
                for (auto& b : bullets) {
                    if (b.active && CheckCollisionRecs(e.GetRect(), {b.pos.x, b.pos.y, 8, 8})) {
                        e.TakeDamage(1); // умирают с одного попадания
                        b.active = false;
                    }
                }
            }
        }

        // ================= 4. ПУЛИ ВРАГОВ =================
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
    srand((unsigned)time(nullptr)); // инициализация rand()
    Init();
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        Update(dt);
        BeginDrawing(); Draw(); EndDrawing();
    }
    delete player; delete map; delete menu;
}