
#include <X11/X.h>

#include <algorithm>

#include <Button.hpp>
#include <Painter.hpp>

namespace HbTK {

Button::Button()
    : Widget(),
      m_text(),
      m_color(48, 54, 64),
      m_hoverColor(62, 72, 86),
      m_pressedColor(35, 41, 50),
      m_borderColor(82, 92, 106),
      m_font(),
      m_style(ButtonStyle::Modern),
      m_hovered(false),
      m_pressed(false){
}

Button::Button(const std::string& text)
    : Button(){
    m_text = text;
}

void Button::setText(const std::string& text){
    m_text = text;
}

const std::string& Button::text() const{
    return m_text;
}

void Button::setStyle(ButtonStyle style){
    m_style = style;
    applyStyle();
}

ButtonStyle Button::buttonStyle() const{
    return m_style;
}

void Button::applyStyle(){
    switch (m_style) {
    case ButtonStyle::Modern:
        m_color = Colors(48, 54, 64);
        m_hoverColor = Colors(62, 72, 86);
        m_pressedColor = Colors(35, 41, 50);
        m_borderColor = Colors(82, 92, 106);
        break;

    case ButtonStyle::Retro:
        m_color = Colors(192, 192, 192);
        m_hoverColor = Colors(208, 208, 208);
        m_pressedColor = Colors(160, 160, 160);
        m_borderColor = Colors(64, 64, 64);
        break;

    case ButtonStyle::Glass:
        m_color = Colors(43, 58, 76);
        m_hoverColor = Colors(56, 78, 101);
        m_pressedColor = Colors(32, 44, 60);
        m_borderColor = Colors(112, 151, 185);
        break;
    }
}

void Button::setColor(const Colors& color){
    m_color = color;
}

const Colors& Button::color() const{
    return m_color;
}

void Button::setHoverColor(const Colors& color){
    m_hoverColor = color;
}

const Colors& Button::hoverColor() const{
    return m_hoverColor;
}

void Button::setPressedColor(const Colors& color){
    m_pressedColor = color;
}

const Colors& Button::pressedColor() const{
    return m_pressedColor;
}

void Button::setBorderColor(const Colors& color){
    m_borderColor = color;
}

const Colors& Button::borderColor() const{
    return m_borderColor;
}

void Button::setFont(const std::string& font){
    m_font = font;
}

const std::string& Button::font() const{
    return m_font;
}

bool Button::isHovered() const{
    return m_hovered;
}

bool Button::isPressed() const{
    return m_pressed;
}

void Button::paint(Painter& painter)
{
    if (!visible() || width() <= 0 || height() <= 0)
        return;

    const Colors* background = &m_color;

    if (m_pressed)
        background = &m_pressedColor;
    else if (m_hovered)
        background = &m_hoverColor;

    const int w = width();
    const int h = height();

    auto fillRoundedRect = [&](int x, int y, int rw, int rh,
                               int radius, const Colors& color) {
        if (rw <= 0 || rh <= 0)
            return;

        radius = std::max(0, std::min(radius, std::min(rw, rh) / 2));

        if (radius == 0) {
            painter.fillRect(x, y, rw, rh, color);
            return;
        }

        painter.fillRect(x + radius, y, rw - radius * 2, rh, color);
        painter.fillRect(x, y + radius, rw, rh - radius * 2, color);
        painter.fillCircle(x, y, radius * 2, color);
        painter.fillCircle(x + rw - radius * 2, y, radius * 2, color);
        painter.fillCircle(x, y + rh - radius * 2, radius * 2, color);
        painter.fillCircle(x + rw - radius * 2,
                           y + rh - radius * 2,
                           radius * 2, color);
    };

    if (m_style == ButtonStyle::Retro) {
        painter.fillRect(0, 0, w, h, *background);
        painter.drawRect(0, 0, w - 1, h - 1, m_borderColor);

        if (w > 2 && h > 2) {
            const Colors& topLeft = m_pressed
                ? Colors(96, 96, 96) : Colors(255, 255, 255);
            const Colors& bottomRight = m_pressed
                ? Colors(255, 255, 255) : Colors(96, 96, 96);

            painter.drawRect(1, 1, w - 3, h - 3, topLeft);

            if (w > 4 && h > 4)
                painter.drawRect(2, 2, w - 5, h - 5, bottomRight);
        }
    } else {
        const int radius = std::min(7, h / 2);

        fillRoundedRect(0, 0, w, h, radius, m_borderColor);

        if (w > 2 && h > 2)
            fillRoundedRect(1, 1, w - 2, h - 2,
                            std::max(0, radius - 1), *background);

        if (m_style == ButtonStyle::Glass && !m_pressed &&
            w > 8 && h > 5) {
            fillRoundedRect(3, 2, w - 6, std::max(1, h / 3),
                            std::max(0, radius - 3),
                            Colors(66, 86, 108));
        }
    }

    if (m_text.empty())
        return;

    const std::string previousFont = painter.font();

    if (!m_font.empty() && !painter.setFont(m_font))
        return;

    if (painter.font().empty()) {
        if (!m_font.empty()) {
            painter.resetFont();
            if (!previousFont.empty())
                painter.setFont(previousFont);
        }
        return;
    }

    const int textWidth = painter.textWidth(m_text.c_str());
    const int textHeight = painter.fontAscent() + painter.fontDescent();

    const int textX = (w - textWidth) / 2;
    const int textY = (h - textHeight) / 2 + painter.fontAscent();

    Colors textColor(235, 239, 245);

    if (m_style == ButtonStyle::Retro)
        textColor = Colors(20, 20, 20);

    painter.drawText(textX, textY, m_text.c_str(), textColor);

    if (!m_font.empty()) {
        painter.resetFont();

        if (!previousFont.empty())
            painter.setFont(previousFont);
    }
}

void Button::mousePress(int x, int y, unsigned int button) {
    if (button != Button1 || !contains(x, y))
        return;

    m_pressed = true;
}

void Button::mouseRelease(int x, int y, unsigned int button) {
    if (button != Button1)
        return;

    m_pressed = false;
    m_hovered = contains(x, y);
}

void Button::mouseMove(int x, int y) {
    m_hovered = contains(x, y);
}

bool Button::contains(int x, int y) const {
    return x >= 0 && y >= 0 && x < width() && y < height();
}

} // namespace HbTK
