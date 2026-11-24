#include "Enemy.h"
#include <cmath>

Enemy::Enemy(Vector2 startPos) {
    pos = startPos;
    hp = 30;
    speed = 80.0f;
    active = true;
}

void Enemy::Update(Vector2 playerPos, float dt) {
    if (!active) return;
    // Едем к игроку
    float dx = playerPos.x - pos.x;
    float dy = playerPos.y - pos.y;
    float dist = sqrtf(dx*dx + dy*dy);
    if (dist > 0) {
        pos.x += (dx / dist) * speed * dt;
        pos.y += (dy / dist) * speed * dt;
    }
}

void Enemy::Draw() {
    if (active) {
        DrawRectangle(pos.x - 20, pos.y - 20, 40, 40, RED);
        DrawCircleV(pos, 10, DARKRED);
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
