#pragma once

#include <cstddef>
#include <string>
#include <Colors.hpp>
#include <Painter.hpp>
#include <Widget.hpp>

namespace HbTK {

class TextEdit : public Widget {
public:
    TextEdit(int x, int y, int width, int height);
    ~TextEdit() override;

    void setText(const std::string& text);
    const std::string& text() const;
    void append(const std::string& text);
    void insertText(const std::string& text);
    void clear();

    void setReadOnly(bool readOnly);
    bool readOnly() const;

    void setCursorPosition(std::size_t position);
    std::size_t cursorPosition() const;

    void selectAll();
    void setFocus(bool focus);
    bool hasFocus() const;

    void setColors(const Colors& background,
                   const Colors& foreground,
                   const Colors& selection);

    void paint(Painter& painter) override;
    void mousePress(int x, int y, unsigned int button) override;
    void mouseMove(int x, int y) override;
    void mouseRelease(int x, int y, unsigned int button) override;
    void keyPress(unsigned int keycode) override;

private:
    void eraseSelection();
    void ensureCursorVisible(Painter& painter);
    std::size_t positionAt(int x, int y, Painter& painter) const;

    std::string m_text;
    std::size_t m_cursorPosition;
    std::size_t m_selectionStart;
    std::size_t m_selectionEnd;
    int m_scrollX;
    int m_scrollY;
    bool m_readOnly;
    bool m_focus;
    bool m_selecting;
    Colors m_background;
    Colors m_foreground;
    Colors m_selection;
};

}