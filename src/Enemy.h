#pragma once
#include "raylib.h"

class Enemy {
public:
    Vector2 pos;
    int hp;
    float speed;
    bool active;

    Enemy(Vector2 startPos);
    void Update(Vector2 playerPos, float dt);
    void Draw();
    Rectangle GetRect() const;
    void TakeDamage(int dmg);
};
