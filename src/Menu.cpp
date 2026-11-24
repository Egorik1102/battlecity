#include "Menu.h"
#include <cmath>

// ---------- Конструктор ----------

Menu::Menu(int screenWidth, int screenHeight)
    : screenW(screenWidth), screenH(screenHeight) {
    items = {
        "PLAY",
        "CONTROLS",
        "EXIT"
    };
}

// ---------- Сброс ----------

void Menu::Reset() {
    selectedIndex = 0;
    screen = Screen::Main;
    blinkTimer = 0.0f;
    blinkVisible = true;
}

// ---------- Геометрия пунктов ----------

Rectangle Menu::GetItemRect(int index) const {
    // Пункты меню по центру экрана, шаг 70px.
    const int fontSize = 40;
    const int itemHeight = 70;
    const int startY = screenH / 2 - 20;  // первая кнопка чуть выше центра

    int textWidth = MeasureText(items[index].c_str(), fontSize);
    Rectangle r;
    r.x = (float)(screenW / 2 - textWidth / 2 - 20);
    r.y = (float)(startY + index * itemHeight - 10);
    r.width = (float)(textWidth + 40);
    r.height = (float)(fontSize + 20);
    return r;
}

// ---------- Обновление ----------

MenuAction Menu::Update(float dt) {
    // Мигание выбранного пункта (частота ~ 2 Гц).
    blinkTimer += dt;
    if (blinkTimer >= 0.35f) {
        blinkTimer = 0.0f;
        blinkVisible = !blinkVisible;
    }

    // Если открыт экран управления — там только один выход (Enter/Space/ESC).
    if (screen == Screen::Controls) {
        bool back = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)
                 || IsKeyPressed(KEY_ESCAPE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        if (back) screen = Screen::Main;
        return MenuAction::None;
    }

    // Наведение мыши — обновляем selectedIndex при ховере.
    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < (int)items.size(); ++i) {
        if (CheckCollisionPointRec(mouse, GetItemRect(i))) {
            selectedIndex = i;
            break;
        }
    }

    // Клавиатура: вверх/вниз (стрелки + WASD).
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        selectedIndex--;
        if (selectedIndex < 0) selectedIndex = (int)items.size() - 1;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        selectedIndex++;
        if (selectedIndex >= (int)items.size()) selectedIndex = 0;
    }

    // Подтверждение выбора.
    bool enterPressed = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE);
    bool mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    MenuAction action = MenuAction::None;
    if (enterPressed || (mousePressed && CheckCollisionPointRec(mouse, GetItemRect(selectedIndex)))) {
        switch (selectedIndex) {
            case 0: action = MenuAction::Play;     break;
            case 1: screen = Screen::Controls;     break;  // Открываем экран управления
            case 2: action = MenuAction::Exit;     break;
        }
    }

    return action;
}

// ---------- Отрисовка ----------

void Menu::Draw() {
    if (screen == Screen::Main) {
        DrawMainScreen();
    } else {
        DrawControlsScreen();
    }
}

// Рисует маленький танк-иконку как курсор рядом с выбранным пунктом.
// Координаты — верхний-левый угол гусеницы.
static void DrawTankCursor(Vector2 pos, Color body, Color turret, float scale) {
    // Гусеница (нижняя полоса)
    Rectangle tracks = { pos.x, pos.y + 18 * scale, 36 * scale, 8 * scale };
    DrawRectangleRec(tracks, DARKGRAY);

    // Корпус
    Rectangle bodyR = { pos.x + 2 * scale, pos.y + 4 * scale, 32 * scale, 16 * scale };
    DrawRectangleRec(bodyR, body);

    // Башня
    DrawCircleV({ pos.x + 18 * scale, pos.y + 12 * scale }, 7 * scale, turret);

    // Ствол смотрит вправо
    DrawLineEx(
        { pos.x + 18 * scale, pos.y + 12 * scale },
        { pos.x + 38 * scale, pos.y + 12 * scale },
        4 * scale, body
    );
}

void Menu::DrawMainScreen() {
    // ----- Фон: тёмный с лёгкой "сеткой" как в оригинале -----
    ClearBackground({ 20, 20, 28, 255 });

    // Декоративная рамка по краям экрана
    const int border = 12;
    DrawRectangleLinesEx(
        { (float)border, (float)border, (float)(screenW - border * 2), (float)(screenH - border * 2) },
        4, { 80, 80, 90, 255 }
    );

    // ----- Заголовок "BATTLE CITY" -----
    const char* title1 = "BATTLE";
    const char* title2 = "CITY";
    int titleSize = 80;

    // Лёгкое смещение — эффект "тени" заголовка
    int t1w = MeasureText(title1, titleSize);
    int t2w = MeasureText(title2, titleSize);
    int titleY = 90;

    DrawText(title1, screenW / 2 - t1w / 2 + 4, titleY + 4, titleSize, { 80, 30, 0, 255 });     // тень
    DrawText(title1, screenW / 2 - t1w / 2,     titleY,     titleSize, GOLD);                    // основной

    int cityY = titleY + titleSize + 6;
    DrawText(title2, screenW / 2 - t2w / 2 + 4, cityY + 4, titleSize, { 80, 30, 0, 255 });
    DrawText(title2, screenW / 2 - t2w / 2,     cityY,     titleSize, GOLD);

    // Подзаголовок
    const char* subtitle = "2D TANKS";
    int subSize = 24;
    int subW = MeasureText(subtitle, subSize);
    DrawText(subtitle, screenW / 2 - subW / 2, cityY + titleSize + 12, subSize, LIGHTGRAY);

    // ----- Пункты меню -----
    const int fontSize = 40;
    const int itemHeight = 70;
    const int startY = screenH / 2 - 20;

    Vector2 mouse = GetMousePosition();

    for (int i = 0; i < (int)items.size(); ++i) {
        int textWidth = MeasureText(items[i].c_str(), fontSize);
        int x = screenW / 2 - textWidth / 2;
        int y = startY + i * itemHeight;

        bool isSelected = (i == selectedIndex);
        bool isHovered = CheckCollisionPointRec(mouse, GetItemRect(i));

        // Подсветка прямоугольником при наведении/выборе
        if (isSelected) {
            Rectangle hl = GetItemRect(i);
            DrawRectangleRec(hl, { 40, 60, 30, 200 });
            DrawRectangleLinesEx(hl, 3, LIME);
        } else if (isHovered) {
            Rectangle hl = GetItemRect(i);
            DrawRectangleLinesEx(hl, 2, { 100, 120, 90, 255 });
        }

        // Мигание выбранного пункта: текст мерцает между жёлтым и белым
        Color textColor;
        if (isSelected) {
            textColor = blinkVisible ? YELLOW : WHITE;
        } else {
            textColor = RAYWHITE;
        }

        // Тень текста для читаемости
        DrawText(items[i].c_str(), x + 2, y + 2, fontSize, { 0, 0, 0, 180 });
        DrawText(items[i].c_str(), x, y, fontSize, textColor);

        // Танк-курсор слева от выбранного пункта
        if (isSelected) {
            Vector2 cursorPos = {
                (float)(x - 60),
                (float)(y + fontSize / 2 - 14)
            };
            DrawTankCursor(cursorPos, YELLOW, ORANGE, 1.0f);
        }
    }

    // ----- Подсказки управления внизу -----
    const char* hint1 = "W/S  - select";
    const char* hint2 = "ESC - exit";
    int hintSize = 18;
    int h1w = MeasureText(hint1, hintSize);
    int h2w = MeasureText(hint2, hintSize);
    DrawText(hint1, screenW / 2 - h1w / 2, screenH - 60, hintSize, GRAY);
    DrawText(hint2, screenW / 2 - h2w / 2, screenH - 36, hintSize, DARKGRAY);
}

void Menu::DrawControlsScreen() {
    ClearBackground({ 20, 20, 28, 255 });

    // Рамка
    const int border = 12;
    DrawRectangleLinesEx(
        { (float)border, (float)border, (float)(screenW - border * 2), (float)(screenH - border * 2) },
        4, { 80, 80, 90, 255 }
    );

    // Заголовок
    const char* title = "CONTROLS";
    int titleSize = 56;
    int tw = MeasureText(title, titleSize);
    DrawText(title, screenW / 2 - tw / 2 + 3, 80 + 3, titleSize, { 80, 30, 0, 255 });
    DrawText(title, screenW / 2 - tw / 2,     80,     titleSize, GOLD);

    // Пункты управления: слева — клавиша, справа — описание.
    struct CtrlRow { const char* key; const char* desc; };
    CtrlRow rows[] = {
        { "W / \x18",      "Move up" },
        { "S / \x19",      "Move down" },
        { "A / \x1A",      "Move left" },
        { "D / \x1B",      "Move right" },
        { "SPACE",        "Shoot (max 5)" },
        { "ESC",           "Pause game" },
    };
    const int rowCount = (int)(sizeof(rows) / sizeof(rows[0]));

    const int keySize = 32;
    const int descSize = 28;
    const int rowHeight = 60;
    const int startY = 200;
    const int keyX = screenW / 2 - 280;
    const int descX = screenW / 2 - 130;

    for (int i = 0; i < rowCount; ++i) {
        int y = startY + i * rowHeight;
        // Тень
        DrawText(rows[i].key,  keyX + 2, y + 2, keySize,  { 0, 0, 0, 180 });
        DrawText(rows[i].desc, descX + 2, y + 4, descSize, { 0, 0, 0, 180 });
        // Текст
        DrawText(rows[i].key,  keyX, y, keySize,  ORANGE);
        DrawText(rows[i].desc, descX, y + 2, descSize, RAYWHITE);
    }

    // Подсказка "нажмите для возврата"
    const char* backHint = "[ ENTER / ESC / LMB] - return to menu";
    int bhSize = 20;
    int bhw = MeasureText(backHint, bhSize);
    // Мигание подсказки
    Color bc = blinkVisible ? YELLOW : GRAY;
    DrawText(backHint, screenW / 2 - bhw / 2, screenH - 60, bhSize, bc);
}
