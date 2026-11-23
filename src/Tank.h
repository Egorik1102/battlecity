#pragma once
#include "raylib.h"

class Tank {
public:
    Vector2 pos;
    float bodyAngle;
    float turretAngle;
    int hp;
    float speed;

    Tank(Vector2 startPos);
    void Update(float dt);
    void Draw();
    Rectangle GetRect() const;
    void TakeDamage(int dmg);
};
