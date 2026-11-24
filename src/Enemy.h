#pragma once
#include "raylib.h"

class Enemy {
public:
    Vector2 pos;
    float bodyAngle;
    int hp;
    float speed;
    bool active;
    float shootCooldown;
    float moveTimer;

    Enemy(Vector2 startPos);
    void Update(Vector2 playerPos, float dt);
    void Draw();
    Rectangle GetRect() const;
    void TakeDamage(int dmg);
};
