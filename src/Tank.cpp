#include "Tank.h"
#include "raylib.h"
#include <cmath>

Tank::Tank(Vector2 startPos) {
    pos = startPos;
    bodyAngle = 0.0f;
    turretAngle = 0.0f;
    hp = 100;
    speed = 200.0f;
}

void Tank::Update(float dt) {
    if (IsKeyDown(KEY_W)) pos.y -= speed * dt;
    if (IsKeyDown(KEY_S)) pos.y += speed * dt;
    if (IsKeyDown(KEY_A)) pos.x -= speed * dt;
    if (IsKeyDown(KEY_D)) pos.x += speed * dt;

    if (IsKeyDown(KEY_Q)) bodyAngle -= 2.5f * dt;
    if (IsKeyDown(KEY_E)) bodyAngle += 2.5f * dt;

    Vector2 mouse = GetMousePosition();
    turretAngle = atan2f(mouse.y - pos.y, mouse.x - pos.x);
}

void Tank::Draw() {
    Rectangle body = { pos.x - 20, pos.y - 20, 40, 40 };
    DrawRectanglePro(body, { 20, 20 }, bodyAngle * RAD2DEG, DARKGREEN);
    DrawCircleV(pos, 10, GREEN);
    
    Vector2 barrelEnd = {
        pos.x + cosf(turretAngle) * 30,
        pos.y + sinf(turretAngle) * 30
    };
    DrawLineEx(pos, barrelEnd, 4.0f, LIME);
}

Rectangle Tank::GetRect() const {
    return { pos.x - 20, pos.y - 20, 40, 40 };
}

void Tank::TakeDamage(int dmg) {
    hp -= dmg;
    if (hp < 0) hp = 0;
}
