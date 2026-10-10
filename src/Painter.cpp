#include <Painter.hpp>

namespace HbTK {

Painter::Painter(Display* display,
                 ::Window window,
                 GC graphicsContext)
    : m_display(display),
      m_window(window),
      m_graphicsContext(graphicsContext),
      m_originX(0),
      m_originY(0),
      m_font(nullptr),
      m_fontName()
{
}

Painter::~Painter()
{
    resetFont();
}

void Painter::translate(int x, int y)
{
    m_originX += x;
    m_originY += y;
}

void Painter::fillRect(int x,
                       int y,
                       unsigned int width,
                       unsigned int height,
                       const Colors& color)
{
    XSetForeground(
        m_display,
        m_graphicsContext,
        pixel(color)
        );

    XFillRectangle(
        m_display,
        m_window,
        m_graphicsContext,
        x + m_originX,
        y + m_originY,
        width,
        height
        );
}

void Painter::drawRect(int x,
                       int y,
                       unsigned int width,
                       unsigned int height,
                       const Colors& color)
{
    XSetForeground(
        m_display,
        m_graphicsContext,
        pixel(color)
        );

    XDrawRectangle(
        m_display,
        m_window,
        m_graphicsContext,
        x + m_originX,
        y + m_originY,
        width,
        height
        );
}

void Painter::drawText(int x,
                       int y,
                       const char* text,
                       const Colors& color)
{
    if (!text)
        return;

    if (!m_font)
        return;

    XftDraw* draw =
        XftDrawCreate(
            m_display,
            m_window,
            DefaultVisual(
                m_display,
                DefaultScreen(m_display)
                ),
            DefaultColormap(
                m_display,
                DefaultScreen(m_display)
                )
            );

    if (!draw)
        return;

    XRenderColor renderColor;

    renderColor.red =
        static_cast<unsigned short>(color.red() * 257);

    renderColor.green =
        static_cast<unsigned short>(color.green() * 257);

    renderColor.blue =
        static_cast<unsigned short>(color.blue() * 257);

    renderColor.alpha = 65535;

    XftColor xftColor;

    if (XftColorAllocValue(
            m_display,
            DefaultVisual(
                m_display,
                DefaultScreen(m_display)
                ),
            DefaultColormap(
                m_display,
                DefaultScreen(m_display)
                ),
            &renderColor,
            &xftColor))
    {
        XftDrawStringUtf8(
            draw,
            &xftColor,
            m_font,
            x + m_originX,
            y + m_originY,
            reinterpret_cast<const FcChar8*>(text),
            static_cast<int>(
                __builtin_strlen(text)
                )
            );

        XftColorFree(
            m_display,
            DefaultVisual(
                m_display,
                DefaultScreen(m_display)
                ),
            DefaultColormap(
                m_display,
                DefaultScreen(m_display)
                ),
            &xftColor
            );
    }

    XftDrawDestroy(draw);
}

int Painter::textWidth(const char* text) const
{
    if (!text || !m_font)
        return 0;

    XGlyphInfo extents;

    XftTextExtentsUtf8(
        m_display,
        m_font,
        reinterpret_cast<const FcChar8*>(text),
        static_cast<int>(__builtin_strlen(text)),
        &extents
        );

    return static_cast<int>(extents.xOff);
}

int Painter::fontAscent() const
{
    if (!m_font)
        return 0;

    return m_font->ascent;
}

int Painter::fontDescent() const
{
    if (!m_font)
        return 0;

    return m_font->descent;
}

bool Painter::setFont(const std::string& font)
{
    if (!m_display)
        return false;

    XftFont* newFont =
        XftFontOpenName(
            m_display,
            DefaultScreen(m_display),
            font.c_str()
            );

    if (!newFont)
        return false;

    resetFont();

    m_font = newFont;
    m_fontName = font;

    return true;
}

void Painter::resetFont()
{
    if (!m_font || !m_display)
        return;

    XftFontClose(
        m_display,
        m_font
        );

    m_font = nullptr;
    m_fontName.clear();
}

const std::string& Painter::font() const
{
    return m_fontName;
}

unsigned long Painter::pixel(const Colors& color)
{
    return (
        (static_cast<unsigned long>(color.red()) << 16) |
        (static_cast<unsigned long>(color.green()) << 8) |
        static_cast<unsigned long>(color.blue())
        );
}

} // namespace HbTK