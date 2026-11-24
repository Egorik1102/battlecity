#pragma once
#include "raylib.h"

class Tank {
public:
    Vector2 pos;
    float bodyAngle; // 0, 90, 180, 270
    int hp;
    float speed;

    Tank(Vector2 startPos);
    void Update(float dt);
    void Draw();
    Rectangle GetRect() const;
    void TakeDamage(int dmg);
};
