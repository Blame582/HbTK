#include <X11/X.h>

#include <Button.hpp>

#include <Painter.hpp>

namespace HbTK {

Button::Button()
    : Widget(),
      m_text(),
      m_color(192, 192, 192),
      m_hoverColor(208, 208, 208),
      m_pressedColor(160, 160, 160),
      m_borderColor(64, 64, 64),
      m_font(),
      m_hovered(false),
      m_pressed(false)
{
}

Button::Button(const std::string& text)
    : Widget(),
      m_text(text),
      m_color(192, 192, 192),
      m_hoverColor(208, 208, 208),
      m_pressedColor(160, 160, 160),
      m_borderColor(64, 64, 64),
      m_font(),
      m_hovered(false),
      m_pressed(false)
{
}

void Button::setText(const std::string& text)
{
    m_text = text;
}

const std::string& Button::text() const
{
    return m_text;
}

void Button::setColor(const Colors& color)
{
    m_color = color;
}

const Colors& Button::color() const
{
    return m_color;
}

void Button::setHoverColor(const Colors& color)
{
    m_hoverColor = color;
}

const Colors& Button::hoverColor() const
{
    return m_hoverColor;
}

void Button::setPressedColor(const Colors& color)
{
    m_pressedColor = color;
}

const Colors& Button::pressedColor() const
{
    return m_pressedColor;
}

void Button::setBorderColor(const Colors& color)
{
    m_borderColor = color;
}

const Colors& Button::borderColor() const
{
    return m_borderColor;
}

void Button::setFont(const std::string& font)
{
    m_font = font;
}

const std::string& Button::font() const
{
    return m_font;
}

bool Button::isHovered() const
{
    return m_hovered;
}

bool Button::isPressed() const
{
    return m_pressed;
}

void Button::paint(Painter& painter)
{
    if (!visible())
        return;

    const Colors* background = &m_color;

    if (m_pressed)
        background = &m_pressedColor;
    else if (m_hovered)
        background = &m_hoverColor;

    painter.fillRect(
        0,
        0,
        width(),
        height(),
        *background
        );

    painter.drawRect(
        0,
        0,
        width() - 1,
        height() - 1,
        m_borderColor
        );

    if (m_text.empty())
        return;

    std::string previousFont = painter.font();

    if (!m_font.empty()) {
        if (!painter.setFont(m_font))
            return;
    }

    if (painter.font().empty())
        return;

    int textWidth =
        painter.textWidth(m_text.c_str());

    int textHeight =
        painter.fontAscent() +
        painter.fontDescent();

    int textX =
        (width() - textWidth) / 2;

    int textY =
        (height() - textHeight) / 2 +
        painter.fontAscent();

    painter.drawText(
        textX,
        textY,
        m_text.c_str(),
        Colors(0, 0, 0)
        );

    if (!m_font.empty()) {
        painter.resetFont();

        if (!previousFont.empty())
            painter.setFont(previousFont);
    }
}

void Button::mousePress(int x,
                        int y,
                        unsigned int button)
{
    if (button != Button1)
        return;

    if (!contains(x, y))
        return;

    m_pressed = true;
}

void Button::mouseRelease(int x,
                          int y,
                          unsigned int button)
{
    if (button != Button1)
        return;

    m_pressed = false;
    m_hovered = contains(x, y);
}

void Button::mouseMove(int x,
                       int y)
{
    m_hovered = contains(x, y);
}

bool Button::contains(int x, int y) const
{
    return x >= 0 &&
           y >= 0 &&
           x < width() &&
           y < height();
}

} // namespace HbTK