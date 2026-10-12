#include <TextEdit.hpp>
#include <Painter.hpp>
#include <Colors.hpp>
#include <algorithm>

namespace HbTK {

TextEdit::TextEdit(int x, int y, int width, int height)
    : m_cursorPosition(0),
      m_selectionStart(0),
      m_selectionEnd(0),
      m_scrollX(0),
      m_scrollY(0),
      m_readOnly(false),
      m_focus(false),
      m_selecting(false),
      m_background(20, 22, 26),
      m_foreground(220, 224, 230),
      m_selection(55, 90, 140) {
    setGeometry(x, y, width, height);
}

TextEdit::~TextEdit() = default;

void TextEdit::setText(const std::string& text) {
    m_text = text;
    m_cursorPosition = std::min(m_cursorPosition, m_text.size());
    m_selectionStart = m_selectionEnd = m_cursorPosition;
    m_scrollX = 0;
    m_scrollY = 0;
}

const std::string& TextEdit::text() const {
    return m_text;
}

void TextEdit::append(const std::string& text) {
    m_text += text;
    m_cursorPosition = m_text.size();
    m_selectionStart = m_selectionEnd = m_cursorPosition;
}

void TextEdit::insertText(const std::string& text) {
    if (m_readOnly)
        return;

    eraseSelection();
    m_text.insert(m_cursorPosition, text);
    m_cursorPosition += text.size();
    m_selectionStart = m_selectionEnd = m_cursorPosition;
}

void TextEdit::clear() {
    m_text.clear();
    m_cursorPosition = 0;
    m_selectionStart = 0;
    m_selectionEnd = 0;
    m_scrollX = 0;
    m_scrollY = 0;
}

void TextEdit::setReadOnly(bool readOnly) {
    m_readOnly = readOnly;
}

bool TextEdit::readOnly() const {
    return m_readOnly;
}

void TextEdit::setCursorPosition(std::size_t position) {
    m_cursorPosition = std::min(position, m_text.size());
    m_selectionStart = m_selectionEnd = m_cursorPosition;
}

std::size_t TextEdit::cursorPosition() const {
    return m_cursorPosition;
}

void TextEdit::selectAll() {
    m_selectionStart = 0;
    m_selectionEnd = m_text.size();
    m_cursorPosition = m_selectionEnd;
}

void TextEdit::setFocus(bool focus) {
    m_focus = focus;
}

bool TextEdit::hasFocus() const {
    return m_focus;
}

void TextEdit::setColors(const Colors& background,
                         const Colors& foreground,
                         const Colors& selection) {
    m_background = background;
    m_foreground = foreground;
    m_selection = selection;
}

void TextEdit::eraseSelection() {
    if (m_selectionStart == m_selectionEnd)
        return;

    const std::size_t first = std::min(m_selectionStart, m_selectionEnd);
    const std::size_t last = std::max(m_selectionStart, m_selectionEnd);

    m_text.erase(first, last - first);
    m_cursorPosition = first;
    m_selectionStart = first;
    m_selectionEnd = first;
}

void TextEdit::ensureCursorVisible(Painter& painter) {
    const int lineHeight = std::max(1, painter.fontAscent() +
                                      painter.fontDescent());
    const int usableHeight = std::max(0, height() - 12);
    const int usableWidth = std::max(0, width() - 12);

    std::size_t lineStart = m_text.rfind(
        '\n', m_cursorPosition == 0 ? 0 : m_cursorPosition - 1);

    if (m_cursorPosition == 0 || lineStart == std::string::npos)
        lineStart = 0;
    else
        ++lineStart;

    const std::string currentLine =
        m_text.substr(lineStart, m_cursorPosition - lineStart);
    const int cursorX = painter.textWidth(currentLine.c_str());

    if (cursorX - m_scrollX > usableWidth)
        m_scrollX = cursorX - usableWidth;
    if (cursorX < m_scrollX)
        m_scrollX = cursorX;

    const int lineNumber = static_cast<int>(
        std::count(m_text.begin(),
                   m_text.begin() + m_cursorPosition, '\n'));
    const int cursorY = lineNumber * lineHeight;

    if (cursorY - m_scrollY > usableHeight)
        m_scrollY = cursorY - usableHeight;
    if (cursorY < m_scrollY)
        m_scrollY = cursorY;

    m_scrollX = std::max(0, m_scrollX);
    m_scrollY = std::max(0, m_scrollY);
}

std::size_t TextEdit::positionAt(int x, int y, Painter& painter) const {
    const int lineHeight = std::max(
        1, painter.fontAscent() + painter.fontDescent());

    const int targetLine = std::max(0, (y - 6 + m_scrollY) / lineHeight);
    const int targetX = std::max(0, x - 6 + m_scrollX);

    std::size_t lineStart = 0;

    for (int line = 0; line < targetLine; ++line) {
        const std::size_t newline = m_text.find('\n', lineStart);

        if (newline == std::string::npos)
            return m_text.size();

        lineStart = newline + 1;
    }

    const std::size_t newline = m_text.find('\n', lineStart);
    const std::size_t lineEnd = newline == std::string::npos
        ? m_text.size() : newline;

    std::size_t position = lineStart;

    while (position < lineEnd) {
        const std::string before =
            m_text.substr(lineStart, position - lineStart);
        const std::string next =
            m_text.substr(lineStart, position - lineStart + 1);

        const int beforeWidth = painter.textWidth(before.c_str());
        const int nextWidth = painter.textWidth(next.c_str());

        if (targetX < beforeWidth + (nextWidth - beforeWidth) / 2)
            return position;

        ++position;
    }

    return lineEnd;
}

void TextEdit::paint(Painter& painter) {
    if (!painter.setFont("monospace:size=12"))
        return;

    const int w = width();
    const int h = height();

    if (w <= 0 || h <= 0)
        return;

    const Colors border = m_focus
        ? Colors(92, 155, 245)
        : Colors(66, 72, 82);

    painter.fillRect(0, 0, w, h, m_background);
    painter.drawRect(0, 0, w, h, border);

    const int padding = 6;
    const int lineHeight = std::max(
        1, painter.fontAscent() + painter.fontDescent());
    const int textX = padding - m_scrollX;
    const int textY = padding + painter.fontAscent() - m_scrollY;
    const int bottom = h - padding;

    std::size_t lineStart = 0;
    int lineNumber = 0;

    while (lineStart <= m_text.size()) {
        const std::size_t lineEnd = m_text.find('\n', lineStart);
        const std::size_t end = lineEnd == std::string::npos
            ? m_text.size() : lineEnd;
        const int y = textY + lineNumber * lineHeight;

        if (y + painter.fontDescent() >= padding && y <= bottom) {
            const std::string line =
                m_text.substr(lineStart, end - lineStart);

            const std::size_t selectionFirst =
                std::min(m_selectionStart, m_selectionEnd);
            const std::size_t selectionLast =
                std::max(m_selectionStart, m_selectionEnd);
            const std::size_t visibleFirst =
                std::max(lineStart, selectionFirst);
            const std::size_t visibleLast =
                std::min(end, selectionLast);

            if (visibleFirst < visibleLast) {
                const std::string before = m_text.substr(
                    lineStart, visibleFirst - lineStart);
                const std::string selected = m_text.substr(
                    visibleFirst, visibleLast - visibleFirst);
                const int sx =
                    textX + painter.textWidth(before.c_str());
                const int sw = painter.textWidth(selected.c_str());

                painter.fillRect(sx, y - painter.fontAscent(),
                                 std::max(1, sw), lineHeight, m_selection);
            }

            if (!line.empty())
                painter.drawText(textX, y, line.c_str(), m_foreground);

            if (m_focus && !m_readOnly &&
                m_cursorPosition >= lineStart &&
                m_cursorPosition <= end) {
                const std::string beforeCursor = m_text.substr(
                    lineStart, m_cursorPosition - lineStart);
                const int cx =
                    textX + painter.textWidth(beforeCursor.c_str());

                painter.fillRect(cx, y - painter.fontAscent(),
                                 1, lineHeight, m_foreground);
            }
        }

        if (lineEnd == std::string::npos)
            break;

        lineStart = lineEnd + 1;
        ++lineNumber;
    }
}

void TextEdit::mousePress(int x, int y, unsigned int button) {
    if (button != 1)
        return;

    m_focus = x >= 0 && y >= 0 && x < width() && y < height();

    if (!m_focus)
        return;

    m_selecting = true;

    m_cursorPosition = std::min(m_cursorPosition, m_text.size());
    m_selectionStart = m_cursorPosition;
    m_selectionEnd = m_cursorPosition;
}

void TextEdit::mouseMove(int x, int y) {
    if (!m_selecting)
        return;

    (void)x;
    (void)y;
}

void TextEdit::mouseRelease(int x, int y, unsigned int button) {
    if (button != 1)
        return;

    m_selecting = false;
    (void)x;
    (void)y;
}

void TextEdit::keyPress(unsigned int keycode) {
    if (!m_focus || m_readOnly)
        return;

    switch (keycode) {
        case 0xff08: // Backspace
            if (m_selectionStart != m_selectionEnd) {
                eraseSelection();
            } else if (m_cursorPosition > 0) {
                m_text.erase(--m_cursorPosition, 1);
                m_selectionStart = m_selectionEnd = m_cursorPosition;
            }
            break;

        case 0xffff: // Delete
            if (m_selectionStart != m_selectionEnd) {
                eraseSelection();
            } else if (m_cursorPosition < m_text.size()) {
                m_text.erase(m_cursorPosition, 1);
            }
            break;

        case 0xff0d: // Enter
        case 0xff8d:
            insertText("\n");
            break;

        case 0xff51: // Left
            if (m_cursorPosition > 0)
                setCursorPosition(m_cursorPosition - 1);
            break;

        case 0xff53: // Right
            if (m_cursorPosition < m_text.size())
                setCursorPosition(m_cursorPosition + 1);
            break;

        case 0xff50: // Home
            setCursorPosition(0);
            break;

        case 0xff57: // End
            setCursorPosition(m_text.size());
            break;

        default:
            break;
    }
}

}