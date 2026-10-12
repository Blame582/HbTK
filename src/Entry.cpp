#include <Entry.hpp>
#include <Painter.hpp>
#include <Colors.hpp>

#include <algorithm>

namespace HbTK {

Entry::Entry(int x, int y, int width, int height)
    : m_cursorPosition(0),
      m_readOnly(false),
      m_focus(false),
      m_style(EntryStyle::Modern),
      m_alignment(EntryAlignment::Left) {
    setGeometry(x, y, width, height);
}

Entry::~Entry() = default;

void Entry::setText(const std::string& text) {
    m_text = text;
    m_cursorPosition = std::min(m_cursorPosition, m_text.size());
}

const std::string& Entry::text() const {
    return m_text;
}

void Entry::clear() {
    m_text.clear();
    m_cursorPosition = 0;
}

void Entry::setLabel(const std::string& label) {
    m_label = label;
}

const std::string& Entry::label() const {
    return m_label;
}

void Entry::setPlaceholder(const std::string& text) {
    m_placeholder = text;
}

const std::string& Entry::placeholder() const {
    return m_placeholder;
}

void Entry::setReadOnly(bool readOnly) {
    m_readOnly = readOnly;
}

bool Entry::readOnly() const {
    return m_readOnly;
}

void Entry::setCursorPosition(std::size_t position) {
    m_cursorPosition = std::min(position, m_text.size());
}

std::size_t Entry::cursorPosition() const {
    return m_cursorPosition;
}

void Entry::setFocus(bool focus) {
    m_focus = focus;
}

bool Entry::hasFocus() const {
    return m_focus;
}

void Entry::setStyle(EntryStyle style) {
    m_style = style;
}

EntryStyle Entry::entryStyle() const {
    return m_style;
}

void Entry::setAlignment(EntryAlignment alignment) {
    m_alignment = alignment;
}

EntryAlignment Entry::alignment() const {
    return m_alignment;
}

int Entry::sizeHintHeight(Painter& painter, int padding) const {
    return painter.fontAscent() + painter.fontDescent() + padding * 2;
}

void Entry::paint(Painter& painter) {
    Colors background(30, 33, 39);
    Colors border = m_focus ? Colors(92, 155, 245) : Colors(66, 72, 82);
    Colors foreground(232, 235, 240);
    Colors placeholderColor(132, 140, 153);

    switch (m_style) {
    case EntryStyle::Modern:
        break;
    case EntryStyle::Retro:
        background = Colors(192, 192, 192);
        border = m_focus ? Colors(40, 80, 160) : Colors(96, 96, 96);
        foreground = Colors(20, 20, 20);
        placeholderColor = Colors(100, 100, 100);
        break;
    case EntryStyle::Glass:
        background = Colors(43, 54, 69);
        border = m_focus ? Colors(105, 220, 245) : Colors(112, 143, 171);
        foreground = Colors(240, 247, 255);
        placeholderColor = Colors(174, 192, 210);
        break;
    }

    auto fillRoundedRect = [&](int x, int y, int w, int h, int radius,
                               const Colors& color) {
        if (w <= 0 || h <= 0)
            return;

        radius = std::max(0, std::min(radius, std::min(w, h) / 2));

        if (radius == 0) {
            painter.fillRect(x, y, w, h, color);
            return;
        }

        painter.fillRect(x + radius, y, w - radius * 2, h, color);
        painter.fillRect(x, y + radius, w, h - radius * 2, color);
        painter.fillCircle(x, y, radius * 2, color);
        painter.fillCircle(x + w - radius * 2, y, radius * 2, color);
        painter.fillCircle(x, y + h - radius * 2, radius * 2, color);
        painter.fillCircle(x + w - radius * 2, y + h - radius * 2,
                           radius * 2, color);
    };

    if (m_style == EntryStyle::Retro) {
        painter.fillRect(0, 0, width(), height(), background);
        painter.drawRect(0, 0, width(), height(), border);

        if (width() > 2 && height() > 2)
            painter.drawRect(1, 1, width() - 2, height() - 2,
                             Colors(230, 230, 230));

        if (width() > 4 && height() > 4)
            painter.drawRect(2, 2, width() - 4, height() - 4,
                             Colors(128, 128, 128));
    } else {
        const int radius = std::min(8, std::min(width(), height()) / 2);

        fillRoundedRect(0, 0, width(), height(), radius, border);

        if (width() > 2 && height() > 2)
            fillRoundedRect(1, 1, width() - 2, height() - 2,
                            std::max(0, radius - 1), background);

        if (m_style == EntryStyle::Glass && width() > 8 && height() > 4)
            fillRoundedRect(3, 2, width() - 6, std::max(1, height() / 3),
                            std::max(0, radius - 3), Colors(52, 67, 84));
    }

    const int padding = 6;
    const int textY = (height() - painter.fontAscent() -
                       painter.fontDescent()) / 2 + painter.fontAscent();

    int textX = padding;

    if (!m_label.empty()) {
        painter.drawText(textX, textY, m_label.c_str(), foreground);
        textX += painter.textWidth(m_label.c_str()) + 8;
    }

    const std::string& shownText = m_text.empty() ? m_placeholder : m_text;
    const Colors& textColor = m_text.empty() ? placeholderColor : foreground;
    const int availableWidth = std::max(0, width() - textX - padding);
    const int shownWidth = painter.textWidth(shownText.c_str());

    if (m_alignment == EntryAlignment::Center)
        textX += std::max(0, (availableWidth - shownWidth) / 2);
    else if (m_alignment == EntryAlignment::Right)
        textX += std::max(0, availableWidth - shownWidth);

    if (!shownText.empty())
        painter.drawText(textX, textY, shownText.c_str(), textColor);

    if (m_focus && !m_readOnly && !m_text.empty()) {
        const std::string beforeCursor = m_text.substr(0, m_cursorPosition);
        const int cursorX = textX + painter.textWidth(beforeCursor.c_str());

        painter.fillRect(cursorX, textY - painter.fontAscent(), 1,
                         painter.fontAscent() + painter.fontDescent(),
                         foreground);
    }
}

void Entry::mousePress(int x, int y, unsigned int button) {
    if (button != 1)
        return;

    m_focus = x >= 0 && y >= 0 && x < width() && y < height();
}

void Entry::keyPress(unsigned int keycode) {
    if (m_readOnly || !m_focus)
        return;

    (void)keycode;
}

} // namespace HbTK