
#pragma once

#include <string>
#include <Colors.hpp>
#include <Widget.hpp>

namespace HbTK {

class Painter;

enum class ButtonStyle {
    Modern,
    Retro,
    Glass
};

class Button : public Widget {
public:
    Button();
    explicit Button(const std::string& text);

    void setText(const std::string& text);
    const std::string& text() const;

    void setStyle(ButtonStyle style);
    ButtonStyle buttonStyle() const;

    void setColor(const Colors& color);
    const Colors& color() const;

    void setHoverColor(const Colors& color);
    const Colors& hoverColor() const;

    void setPressedColor(const Colors& color);
    const Colors& pressedColor() const;

    void setBorderColor(const Colors& color);
    const Colors& borderColor() const;

    void setFont(const std::string& font);
    const std::string& font() const;

    bool isHovered() const;
    bool isPressed() const;

    void paint(Painter& painter) override;

    void mousePress(int x, int y, unsigned int button) override;
    void mouseRelease(int x, int y, unsigned int button) override;
    void mouseMove(int x, int y) override;

private:
    bool contains(int x, int y) const;
    void applyStyle();

    std::string m_text;
    Colors m_color;
    Colors m_hoverColor;
    Colors m_pressedColor;
    Colors m_borderColor;
    std::string m_font;
    ButtonStyle m_style;
    bool m_hovered;
    bool m_pressed;
};

} // namespace HbTK
