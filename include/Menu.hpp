#pragma once

#include "Widget.hpp"
#include "Colors.hpp"

#include <string>
#include <vector>

namespace HbTK {

class Painter;

class Menu : public Widget
{
public:
    struct Item
    {
        std::string text;
        bool separator;
        bool enabled;
    };

    Menu();

    ~Menu() override;

    Menu(const Menu&) = delete;
    Menu& operator=(const Menu&) = delete;

    void addItem(const std::string& text);
    void addSeparator();

    void setItemEnabled(int index, bool enabled);
    bool itemEnabled(int index) const;

    int count() const;
    const Item* item(int index) const;

    void setBackground(const Colors& color);
    const Colors& background() const;

    void setTextColor(const Colors& color);
    const Colors& textColor() const;

    void setDisabledTextColor(const Colors& color);
    const Colors& disabledTextColor() const;

    void setSelectedBackground(const Colors& color);
    const Colors& selectedBackground() const;

    void setSelectedTextColor(const Colors& color);
    const Colors& selectedTextColor() const;

    void setItemHeight(int height);
    int itemHeight() const;

    void show();
    void hide();

    bool isShown() const;

    void mousePress(int x,
                    int y,
                    unsigned int button) override;

    void mouseMove(int x,
                   int y) override;

    void paint(Painter& painter) override;

private:
    int itemAt(int x, int y) const;

    std::vector<Item> m_items;

    Colors m_background;
    Colors m_textColor;
    Colors m_disabledTextColor;
    Colors m_selectedBackground;
    Colors m_selectedTextColor;

    int m_itemHeight;
    int m_selectedItem;
    bool m_shown;
};

} // namespace HbTK