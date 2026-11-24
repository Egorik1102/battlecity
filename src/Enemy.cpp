#include "Enemy.h"
#include "raylib.h"
#include <cmath>

Enemy::Enemy(Vector2 startPos) {
    pos = startPos;
    bodyAngle = 90.0f;
    hp = 30;
    speed = 80.0f;
    active = true;
    shootCooldown = 1.0f;
}

void Enemy::Update(Vector2 playerPos, float dt) {
    if (!active) return;
    
    // Враг смотрит на игрока
    float dx = playerPos.x - pos.x;
    float dy = playerPos.y - pos.y;
    bodyAngle = atan2f(dy, dx) * RAD2DEG;

    // Враг едет к игроку
    float dist = sqrtf(dx*dx + dy*dy);
    if (dist > 0) {
        pos.x += (dx / dist) * speed * dt;
        pos.y += (dy / dist) * speed * dt;
    }

    // Кулдаун стрельбы
    shootCooldown -= dt;
}

void Enemy::Draw() {
    if (active) {
        // Рисуем как танк
        Rectangle body = { pos.x, pos.y, 40, 40 };
        DrawRectanglePro(body, { 20, 20 }, bodyAngle + 90.0f, RED);
        
        float radians = bodyAngle * DEG2RAD;
        Vector2 barrelEnd = { pos.x + cosf(radians) * 25, pos.y + sinf(radians) * 25 };
        DrawLineEx(pos, barrelEnd, 6.0f, YELLOW);
        DrawCircleV(pos, 10, MAROON);
        
        DrawText(TextFormat("%d", hp), pos.x - 10, pos.y - 40, 20, WHITE);
    }
}

Rectangle Enemy::GetRect() const {
    return { pos.x - 20, pos.y - 20, 40, 40 };
}

void Enemy::TakeDamage(int dmg) {
    hp -= dmg;
    if (hp <= 0) active = false;
}
