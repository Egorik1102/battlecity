#include "Tank.h"
#include "raylib.h"
#include <cmath>

Tank::Tank(Vector2 startPos) {
    pos = startPos;
    bodyAngle = -90.0f; // По умолчанию смотрим вверх
    hp = 100;
    speed = 150.0f;
}

void Tank::Update(float dt) {
    // Запоминаем, двигался ли танк в этом кадре
    bool isMoving = false;

    // 1. Смена направления (стреляем по кнопкам)
    if (IsKeyPressed(KEY_W) || IsKeyDown(KEY_W)) { bodyAngle = -90.0f; isMoving = true; }
    if (IsKeyPressed(KEY_S) || IsKeyDown(KEY_S)) { bodyAngle = 90.0f;  isMoving = true; }
    if (IsKeyPressed(KEY_A) || IsKeyDown(KEY_A)) { bodyAngle = 180.0f; isMoving = true; }
    if (IsKeyPressed(KEY_D) || IsKeyDown(KEY_D)) { bodyAngle = 0.0f;   isMoving = true; }

    // 2. Движение вперед, если нажата кнопка
    if (isMoving) {
        float radians = bodyAngle * DEG2RAD;
        // Двигаем танк в ту сторону, куда смотрит пушка
        pos.x += cosf(radians) * speed * dt;
        pos.y += sinf(radians) * speed * dt;
    }
}

void Tank::Draw() {
    // Рисуем корпус (квадрат). pos - это центр.
    Rectangle body = { pos.x, pos.y, 40, 40 }; 
    DrawRectanglePro(body, { 20, 20 }, bodyAngle + 90.0f, DARKGREEN); 
    
    // Рисуем пушку (ствол) ровно из центра
    float radians = bodyAngle * DEG2RAD;
    Vector2 barrelEnd = { pos.x + cosf(radians) * 25, pos.y + sinf(radians) * 25 };
    
    DrawLineEx(pos, barrelEnd, 6.0f, LIME);
    DrawCircleV(pos, 10, GREEN); // Башня
}

Rectangle Tank::GetRect() const {
    return { pos.x - 20, pos.y - 20, 40, 40 };
}

void Tank::TakeDamage(int dmg) {
    hp -= dmg;
    if (hp < 0) hp = 0;
}
