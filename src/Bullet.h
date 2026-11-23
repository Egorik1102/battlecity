#pragma once
#include "raylib.h"

class Bullet {
public:
    Vector2 pos;
    float angle;
    float speed;
    bool active;

    Bullet(Vector2 startPos, float startAngle);
    void Update(float dt);
    void Draw();
};
