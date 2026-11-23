#include "Game.h"
#include "raylib.h"
#include <algorithm>

Game::Game() {
    menu = new Menu(screenWidth, screenHeight);
}

void Game::Init() {
    player = new Tank({ 640, 360 });
    map = new Map();
    map->Init();
}

// Сброс уровня — вызывается при старте новой игры и при Game Over -> Play.
void Game::Restart() {
    delete player;
    delete map;
    player = nullptr;
    map = nullptr;
    bullets.clear();
    Init();
}

void Game::Update(float dt) {
    // ----- Главное меню -----
    if (state == GameState::Menu) {
        MenuAction action = menu->Update(dt);
        switch (action) {
            case MenuAction::Play:
                Restart();
                state = GameState::Playing;
                break;
            case MenuAction::Controls:
                // Открывается внутри Menu — здесь ничего не делаем.
                break;
            case MenuAction::Exit:
                // Корректно закрываем окно raylib.
                CloseWindow();
                break;
            case MenuAction::None:
            default:
                break;
        }
        return;
    }

    // ----- Пауза -----
    if (state == GameState::Paused) {
        // ESC в паузе — продолжить игру.
        if (IsKeyPressed(KEY_ESCAPE)) {
            state = GameState::Playing;
        }
        // ENTER в паузе — выйти в главное меню.
        if (IsKeyPressed(KEY_ENTER)) {
            state = GameState::Menu;
            menu->Reset();
        }
        return;
    }

    // ----- Игра -----
    if (state == GameState::Playing) {
        // ESC во время игры — пауза.
        if (IsKeyPressed(KEY_ESCAPE)) {
            state = GameState::Paused;
            return;
        }

        if (player) {
            Vector2 oldPos = player->pos;
            player->Update(dt);

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

        for (auto& b : bullets) b.Update(dt);
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b) { return !b.active; }), bullets.end());
    }
}

void Game::Draw() {
    ClearBackground({ 30, 30, 35, 255 });

    // ----- Меню рисуется само (со своим фоном) -----
    if (state == GameState::Menu) {
        menu->Draw();
        return;
    }

    if (state == GameState::Playing || state == GameState::Paused) {
        if (map) map->Draw();
        if (player) player->Draw();
        for (auto& b : bullets) b.Draw();

        // HUD-подсказка
        DrawText("WASD - move | Space - shoot | ESC - pause",
                 20, 20, 20, LIGHTGRAY);

        // Затемнение и надпись при паузе
        if (state == GameState::Paused) {
            DrawRectangle(0, 0, screenWidth, screenHeight, { 0, 0, 0, 160 });

            const char* pauseText = "ПАУЗА";
            int pauseSize = 72;
            int pw = MeasureText(pauseText, pauseSize);
            DrawText(pauseText, screenWidth / 2 - pw / 2 + 4,
                     screenHeight / 2 - 60 + 4, pauseSize, { 60, 60, 0, 255 });
            DrawText(pauseText, screenWidth / 2 - pw / 2,
                     screenHeight / 2 - 60, pauseSize, YELLOW);

            const char* hint = "ESC - продолжить     ENTER - в меню";
            int hintSize = 24;
            int hw = MeasureText(hint, hintSize);
            DrawText(hint, screenWidth / 2 - hw / 2,
                     screenHeight / 2 + 40, hintSize, RAYWHITE);
        }
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
