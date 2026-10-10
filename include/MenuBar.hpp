#pragma once

#include "Widget.hpp"
#include "Colors.hpp"
#include "Menu.hpp"

#include <string>
#include <vector>

namespace HbTK {

class Painter;

class MenuBar : public Widget
{
public:
    struct Entry
    {
        std::string text;
        Menu* menu;
    };

    MenuBar();

    ~MenuBar() override;

    MenuBar(const MenuBar&) = delete;
    MenuBar& operator=(const MenuBar&) = delete;

    Menu* addMenu(const std::string& text);

    void removeMenu(Menu* menu);

    int count() const;
    Menu* menu(int index) const;
    const Entry* entry(int index) const;

    void setBackground(const Colors& color);
    const Colors& background() const;

    void setTextColor(const Colors& color);
    const Colors& textColor() const;

    void setSelectedBackground(const Colors& color);
    const Colors& selectedBackground() const;

    void setSelectedTextColor(const Colors& color);
    const Colors& selectedTextColor() const;

    void setHeight(int height);
    int barHeight() const;

    void mousePress(int x,
                    int y,
                    unsigned int button) override;

    void mouseMove(int x,
                   int y) override;

    void paint(Painter& painter) override;

private:
    int menuAt(int x, int y) const;

    void showMenu(int index);
    void hideMenus();

    std::vector<Entry> m_entries;

    Colors m_background;
    Colors m_textColor;
    Colors m_selectedBackground;
    Colors m_selectedTextColor;

    int m_barHeight;
    int m_selectedMenu;
};

} // namespace HbTK