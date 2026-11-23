#include "Bullet.h"
#include <cmath>

Bullet::Bullet(Vector2 startPos, float startAngle) {
    pos = startPos;
    angle = startAngle;
    speed = 400.0f;
    active = true;
}

void Bullet::Update(float dt) {
    if (!active) return;
    float radians = angle * DEG2RAD;
    pos.x += cosf(radians) * speed * dt;
    pos.y += sinf(radians) * speed * dt;
    
    if (pos.x < 0 || pos.x > 1280 || pos.y < 0 || pos.y > 720) {
        active = false;
    }
}

void Bullet::Draw() {
    if (active) {
        DrawCircleV(pos, 4, YELLOW);
    }
}
