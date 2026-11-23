#pragma once
#include "raylib.h"
#include <vector>

class Map {
public:
    std::vector<Rectangle> walls;
    void Init();
    void Draw();
};
