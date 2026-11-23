#include "Map.h"

void Map::Init() {
    // Тестовые стены
    walls = {
        { 200, 200, 100, 20 },
        { 500, 300, 20, 200 },
        { 800, 100, 150, 20 },
        { 100, 500, 200, 20 }
    };
}

void Map::Draw() {
    for (const auto& wall : walls) {
        DrawRectangleRec(wall, DARKGRAY);
        DrawRectangleLinesEx(wall, 2, GRAY);
    }
}
