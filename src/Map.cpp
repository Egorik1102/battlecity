#include "Map.h"

static const char* DEFAULT_MAP[Map::MAP_ROWS] = {
    "SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS",
    "S..............................S",
    "S..BBBB....BB.....BBBB...BBBB..S",
    "S..BBBB....BB.....BBBB...BBBB..S",
    "S..........BB..................S",
    "S..........BB..................S",
    "S..BBBB..BBBB..BBBB..BBBB..BB..S",
    "S..BBBB..BBBB..BBBB..BBBB..BB..S",
    "S..............................S",
    "S..............................S",
    "S..BBBB..BBBB..BBBB..BBBB......S",
    "S..BBBB..BBBB..BBBB..BBBB......S",
    "S..............................S",
    "S.....BB....BBBB...BBBB..BB....S",
    "S.....BB....BBBB...BBBB..BB....S",
    "S..............................S",
    "S..............................S",
    "SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS"
};

void Map::Init() {
    offset.x = 0.0f; offset.y = 0.0f;
    for (int row = 0; row < MAP_ROWS; ++row) {
        for (int col = 0; col < MAP_COLS; ++col) {
            char c = DEFAULT_MAP[row][col];
            if (c == 'B') tiles[row][col] = TILE_BRICK;
            else if (c == 'S') tiles[row][col] = TILE_STEEL;
            else tiles[row][col] = TILE_EMPTY;
        }
    }
}

namespace {
void DrawBrickTile(Rectangle r) {
    DrawRectangleRec(r, { 180, 70, 50, 255 });
    DrawRectangleLinesEx(r, 1, { 90, 30, 20, 255 });
    DrawLine((int)r.x, (int)(r.y + r.height / 2), (int)(r.x + r.width), (int)(r.y + r.height / 2), { 90, 30, 20, 255 });
    DrawLine((int)(r.x + r.width / 2), (int)r.y, (int)(r.x + r.width / 2), (int)(r.y + r.height / 2), { 90, 30, 20, 255 });
    DrawLine((int)r.x, (int)(r.y + r.height / 2), (int)r.x, (int)(r.y + r.height), { 90, 30, 20, 255 });
    DrawLine((int)(r.x + r.width), (int)(r.y + r.height / 2), (int)(r.x + r.width), (int)(r.y + r.height), { 90, 30, 20, 255 });
}
void DrawSteelTile(Rectangle r) {
    DrawRectangleRec(r, { 160, 160, 170, 255 });
    DrawRectangleLinesEx(r, 2, { 70, 70, 80, 255 });
    Rectangle inner = { r.x + 4, r.y + 4, r.width - 8, r.height - 8 };
    DrawRectangleLinesEx(inner, 1, { 110, 110, 120, 255 });
}
}

void Map::Draw() {
    Rectangle field = { offset.x, offset.y, (float)GetWidthPx(), (float)GetHeightPx() };
    DrawRectangleRec(field, { 10, 10, 10, 255 });
    for (int row = 0; row < MAP_ROWS; ++row) {
        for (int col = 0; col < MAP_COLS; ++col) {
            Rectangle r = GetTileRect(col, row);
            if (tiles[row][col] == TILE_BRICK) DrawBrickTile(r);
            else if (tiles[row][col] == TILE_STEEL) DrawSteelTile(r);
        }
    }
}

bool Map::CheckRectCollision(Rectangle rect) const {
    int minCol = (int)((rect.x - offset.x) / TILE_SIZE);
    int maxCol = (int)((rect.x + rect.width  - 1 - offset.x) / TILE_SIZE);
    int minRow = (int)((rect.y - offset.y) / TILE_SIZE);
    int maxRow = (int)((rect.y + rect.height - 1 - offset.y) / TILE_SIZE);
    if (minCol < 0) minCol = 0; if (minRow < 0) minRow = 0;
    if (maxCol >= MAP_COLS) maxCol = MAP_COLS - 1; if (maxRow >= MAP_ROWS) maxRow = MAP_ROWS - 1;
    for (int row = minRow; row <= maxRow; ++row)
        for (int col = minCol; col <= maxCol; ++col)
            if (tiles[row][col] == TILE_BRICK || tiles[row][col] == TILE_STEEL) return true;
    return false;
}

bool Map::IsSolidAt(float worldX, float worldY) const {
    int col = (int)((worldX - offset.x) / TILE_SIZE);
    int row = (int)((worldY - offset.y) / TILE_SIZE);
    if (col < 0 || col >= MAP_COLS || row < 0 || row >= MAP_ROWS) return false;
    return (tiles[row][col] == TILE_BRICK || tiles[row][col] == TILE_STEEL);
}

bool Map::DamageTile(int col, int row) {
    if (col < 0 || col >= MAP_COLS || row < 0 || row >= MAP_ROWS) return false;
    if (tiles[row][col] == TILE_BRICK) { tiles[row][col] = TILE_EMPTY; return true; }
    return false;
}

Rectangle Map::GetTileRect(int col, int row) const {
    return { offset.x + col * TILE_SIZE, offset.y + row * TILE_SIZE, (float)TILE_SIZE, (float)TILE_SIZE };
}

void Map::WorldToTile(float worldX, float worldY, int& col, int& row) const {
    col = (int)((worldX - offset.x) / TILE_SIZE);
    row = (int)((worldY - offset.y) / TILE_SIZE);
}
