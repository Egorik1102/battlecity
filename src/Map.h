#pragma once
#include "raylib.h"

enum TileType {
    TILE_EMPTY = 0,   // пусто
    TILE_BRICK = 1,   // кирпич (разрушаемый)
    TILE_STEEL = 2    // сталь (неразрушаемый)
};

class Map {
public:
    static const int MAP_COLS = 32;
    static const int MAP_ROWS = 18;
    static const int TILE_SIZE = 40;

    int tiles[MAP_ROWS][MAP_COLS];
    Vector2 offset;

    void Init();
    void Draw();
    bool CheckRectCollision(Rectangle rect) const;
    bool IsSolidAt(float worldX, float worldY) const;
    bool DamageTile(int col, int row);
    Rectangle GetTileRect(int col, int row) const;
    void WorldToTile(float worldX, float worldY, int& col, int& row) const;
    int GetWidthPx()  const { return MAP_COLS * TILE_SIZE; }
    int GetHeightPx() const { return MAP_ROWS * TILE_SIZE; }
};
