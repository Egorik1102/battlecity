#pragma once
#include "raylib.h"
#include <vector>
#include <string>

// Действия, которые меню может вернуть наружу.
// Game сам решает, как реагировать на каждое из них.
enum class MenuAction {
    None,        // Ничего не нажато
    Play,        // Начать игру
    Controls,    // Открыть экран управления
    Exit         // Выйти из игры
};

// Главное меню игры. Рисуется поверх фона Game.
// Управление:
//   W/Стрелка вверх  - предыдущий пункт
//   S/Стрелка вниз   - следующий пункт
//   Enter/Space      - выбрать
//   Мышь             - наведение + ЛКМ выбор
class Menu {
public:
    Menu(int screenWidth, int screenHeight);

    // Обновляет состояние меню и возвращает действие,
    // которое нужно выполнить наружу (или None).
    MenuAction Update(float dt);

    // Рисует меню целиком: фон, заголовок, пункты, танк-курсор, подсказки.
    void Draw();

    // Сброс выбранного пункта на "ИГРАТЬ" (вызывать при возврате в меню).
    void Reset();

    // Геттер для текущего выбранного пункта (для отладки/тестов).
    int GetSelectedIndex() const { return selectedIndex; }

private:
    // Перечисление экранов меню. Сейчас два: главное и управление.
    enum class Screen { Main, Controls };

    void DrawMainScreen();
    void DrawControlsScreen();

    // Возвращает прямоугольник пункта меню по индексу (для клика мышью).
    Rectangle GetItemRect(int index) const;

    int screenW;
    int screenH;

    Screen screen = Screen::Main;

    std::vector<std::string> items;
    int selectedIndex = 0;

    // Таймер для мигания выбранного пункта.
    float blinkTimer = 0.0f;
    bool blinkVisible = true;

    // Чтобы Enter не срабатывал дважды на одно нажатие.
    bool enterWasPressed = false;
    bool mouseWasPressed = false;
};
