#include <Label.hpp>

#include <Painter.hpp>

namespace HbTK {

Label::Label()
    : Widget(),
      m_text(),
      m_alignment(Alignment::Left),
      m_color(),
      m_font()
{
}

Label::Label(const std::string& text)
    : Widget(),
      m_text(text),
      m_alignment(Alignment::Left),
      m_color(),
      m_font()
{
}

void Label::setText(const std::string& text)
{
    m_text = text;
}

const std::string& Label::text() const
{
    return m_text;
}

void Label::setAlignment(Alignment alignment)
{
    m_alignment = alignment;
}

Label::Alignment Label::alignment() const
{
    return m_alignment;
}

void Label::setColor(const Colors& color)
{
    m_color = color;
}

const Colors& Label::color() const
{
    return m_color;
}

void Label::setFont(const std::string& font)
{
    m_font = font;
}

const std::string& Label::font() const
{
    return m_font;
}

void Label::paint(Painter& painter)
{
    if (!visible())
        return;

    if (m_text.empty())
        return;

    std::string previousFont = painter.font();

    if (!m_font.empty()) {
        if (!painter.setFont(m_font))
            return;
    }

    if (painter.font().empty())
        return;

    int textWidth = painter.textWidth(m_text.c_str());

    int textX = 0;

    switch (m_alignment) {
    case Alignment::Left:
        textX = 0;
        break;

    case Alignment::Center:
        textX = (width() - textWidth) / 2;
        break;

    case Alignment::Right:
        textX = width() - textWidth;
        break;
    }

    int textHeight =
        painter.fontAscent() +
        painter.fontDescent();

    int textY =
        (height() - textHeight) / 2 +
        painter.fontAscent();

    painter.drawText(
        textX,
        textY,
        m_text.c_str(),
        m_color
        );

    if (!m_font.empty()) {
        painter.resetFont();

        if (!previousFont.empty())
            painter.setFont(previousFont);
    }
}

} // namespace HbTK