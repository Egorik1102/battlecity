#pragma once
#include "raylib.h"

enum class Owner { Player, Enemy };

struct Bullet {
    Vector2 pos;
    Vector2 vel;
    float life;
    Owner owner;
};

struct TankData {
    Vector2 pos;
    float angle;
    float turretAngle;
    int hp;
    Rectangle GetRect() const { return { pos.x - 20, pos.y - 20, 40, 40 }; }
};

struct EnemyData {
    Vector2 pos;
    int hp;
    Rectangle GetRect() const { return { pos.x - 20, pos.y - 20, 40, 40 }; }
};
