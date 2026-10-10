#pragma once

#include <string>

#include <Colors.hpp>
#include <Widget.hpp>

namespace HbTK {

class Painter;

class Label : public Widget {
public:
    enum class Alignment {
        Left,
        Center,
        Right
    };

    Label();
    explicit Label(const std::string& text);

    void setText(const std::string& text);
    const std::string& text() const;

    void setAlignment(Alignment alignment);
    Alignment alignment() const;

    void setColor(const Colors& color);
    const Colors& color() const;

    void setFont(const std::string& font);
    const std::string& font() const;

    void paint(Painter& painter) override;

private:
    std::string m_text;
    Alignment m_alignment;
    Colors m_color;
    std::string m_font;
};

} // namespace HbTK