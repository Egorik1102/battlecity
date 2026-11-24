#include "Enemy.h"
#include "raylib.h"
#include <cmath>
#include <cstdlib>

Enemy::Enemy(Vector2 startPos) {
    pos = startPos;
    bodyAngle = -90.0f;
    hp = 30;
    speed = 80.0f;
    active = true;
    shootCooldown = 1.5f;
    moveTimer = 0.0f;
}

void Enemy::Update(Vector2 playerPos, float dt) {
    if (!active) return;

    moveTimer -= dt;
    if (moveTimer <= 0.0f) {
        moveTimer = 2.0f;
        int dir = rand() % 4;
        bodyAngle = dir * 90.0f;
    }

    float radians = bodyAngle * DEG2RAD;
    Vector2 nextPos = pos;
    nextPos.x += cosf(radians) * speed * dt;
    nextPos.y += sinf(radians) * speed * dt;

    if (nextPos.x < 20 || nextPos.x > 1260 || nextPos.y < 20 || nextPos.y > 700) {
        moveTimer = 0.0f;
    } else {
        pos = nextPos;
    }

    shootCooldown -= dt;
}

void Enemy::Draw() {
    if (active) {
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
