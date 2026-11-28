#include "Enemy.h"
#include "raylib.h"
#include <cmath>
#include <cstdlib>

Enemy::Enemy(Vector2 startPos) {
    pos = startPos;
    bodyAngle = -90.0f;
    hp = 1;
    speed = 80.0f;
    active = true;
    shootCooldown = 1.5f;
    moveTimer = 0.0f;
    spawnTimer = 1.0f; // 1 секунда на анимацию спавна
}

void Enemy::Update(Vector2 playerPos, float dt) {
    if (!active) return;

    if (spawnTimer > 0.0f) {
        spawnTimer -= dt;
        return; // Пока идет анимация - враг стоит на месте
    }

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
        if (spawnTimer > 0.0f) {
            // Анимация появления (3-4 кадра)
            float frame = fmodf(1.0f - spawnTimer, 0.2f) * 5.0f;
            int f = (int)frame % 3; // 0, 1, 2
            
            // Рисуем звездочку (крестик)
            Vector2 c = pos;
            if (f == 0) DrawLineEx({c.x-20, c.y}, {c.x+20, c.y}, 4, WHITE);
            if (f == 1) DrawLineEx({c.x, c.y-20}, {c.x, c.y+20}, 4, WHITE);
            if (f == 2) { DrawLineEx({c.x-20, c.y}, {c.x+20, c.y}, 4, WHITE); DrawLineEx({c.x, c.y-20}, {c.x, c.y+20}, 4, WHITE); }
            return;
        }

        Rectangle body = { pos.x, pos.y, 40, 40 };
        DrawRectanglePro(body, { 20, 20 }, bodyAngle + 90.0f, RED);
        
        float radians = bodyAngle * DEG2RAD;
        Vector2 barrelEnd = { pos.x + cosf(radians) * 25, pos.y + sinf(radians) * 25 };
        DrawLineEx(pos, barrelEnd, 6.0f, YELLOW);
        DrawCircleV(pos, 10, MAROON);
        // Цифры HP убрали
    }
}

Rectangle Enemy::GetRect() const { return { pos.x - 20, pos.y - 20, 40, 40 }; }
void Enemy::TakeDamage(int dmg) { hp -= dmg; if (hp <= 0) active = false; }
