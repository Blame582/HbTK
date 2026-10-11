
#pragma once

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

#include <string>
#include <Colors.hpp>

namespace HbTK {

class Painter {
public:
    Painter(Display* display,
            ::Window window,
            GC graphicsContext,
            Visual* visual = nullptr,
            Colormap colormap = 0);

    ~Painter();

    void translate(int x, int y);

    void fillRect(int x, int y,
                  unsigned int width, unsigned int height,
                  const Colors& color);

    void drawRect(int x, int y,
                  unsigned int width, unsigned int height,
                  const Colors& color);

    void drawText(int x, int y,
                  const char* text,
                  const Colors& color);

    int textWidth(const char* text) const;
    int fontAscent() const;
    int fontDescent() const;

    bool setFont(const std::string& font);
    void resetFont();

    const std::string& font() const;

private:
    unsigned long pixel(const Colors& color);

    Display* m_display;
    ::Window m_window;
    GC m_graphicsContext;
    Visual* m_visual;
    Colormap m_colormap;

    int m_originX;
    int m_originY;

    XftFont* m_font;
    std::string m_fontName;
};

} // namespace HbTK
