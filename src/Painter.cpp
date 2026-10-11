
#include <Painter.hpp>

#include <X11/extensions/Xrender.h>
#include <X11/Xutil.h>

#include <cstring>

namespace HbTK {

Painter::Painter(Display* display,
                 ::Window window,
                 GC graphicsContext,
                 Visual* visual,
                 Colormap colormap)
    : m_display(display),
      m_window(window),
      m_graphicsContext(graphicsContext),
      m_visual(visual),
      m_colormap(colormap),
      m_originX(0),
      m_originY(0),
      m_font(nullptr),
      m_fontName()
{
    if (!m_display)
        return;

    const int screen = DefaultScreen(m_display);

    if (!m_visual)
        m_visual = DefaultVisual(m_display, screen);

    if (!m_colormap)
        m_colormap = DefaultColormap(m_display, screen);
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

void Painter::setOrigin(int x, int y)
{
    m_originX = x;
    m_originY = y;
}

int Painter::originX() const
{
    return m_originX;
}

int Painter::originY() const
{
    return m_originY;
}

void Painter::fillRect(int x, int y,
                       unsigned int width, unsigned int height,
                       const Colors& color)
{
    if (!m_display || !m_window || !width || !height)
        return;

    XRenderPictFormat* format = XRenderFindVisualFormat(
        m_display, m_visual);

    if (!format)
        return;

    Picture destination = XRenderCreatePicture(
        m_display, m_window, format, 0, nullptr);

    if (!destination)
        return;

    XRenderColor renderColor;
    renderColor.red = static_cast<unsigned short>(color.red() * 257);
    renderColor.green = static_cast<unsigned short>(color.green() * 257);
    renderColor.blue = static_cast<unsigned short>(color.blue() * 257);
    renderColor.alpha = static_cast<unsigned short>(color.alpha() * 257);

    XRenderFillRectangle(
        m_display,
        PictOpOver,
        destination,
        &renderColor,
        x + m_originX,
        y + m_originY,
        width,
        height);

    XRenderFreePicture(m_display, destination);
}

void Painter::drawRect(int x, int y,
                       unsigned int width, unsigned int height,
                       const Colors& color)
{
    if (!width || !height)
        return;

    fillRect(x, y, width, 1, color);

    if (height > 1)
        fillRect(x, y + static_cast<int>(height) - 1,
                 width, 1, color);

    if (height > 2) {
        fillRect(x, y + 1, 1, height - 2, color);

        if (width > 1)
            fillRect(x + static_cast<int>(width) - 1,
                     y + 1, 1, height - 2, color);
    }
}

void Painter::fillCircle(int x, int y,
                         unsigned int diameter,
                         const Colors& color)
{
    if (!m_display || !m_window || !m_graphicsContext || !diameter)
        return;

    XSetForeground(m_display, m_graphicsContext, pixel(color));
    XFillArc(m_display, m_window, m_graphicsContext,
             x + m_originX, y + m_originY,
             diameter, diameter, 0, 360 * 64);
}

void Painter::drawCircle(int x, int y,
                         unsigned int diameter,
                         const Colors& color)
{
    if (!m_display || !m_window || !m_graphicsContext || !diameter)
        return;

    XSetForeground(m_display, m_graphicsContext, pixel(color));
    XDrawArc(m_display, m_window, m_graphicsContext,
             x + m_originX, y + m_originY,
             diameter - 1, diameter - 1, 0, 360 * 64);
}

void Painter::fillHalfCircle(int x, int y,
                             unsigned int diameter,
                             bool leftHalf,
                             const Colors& color)
{
    if (!m_display || !m_window || !m_graphicsContext || !diameter)
        return;

    const int startAngle = leftHalf ? 90 * 64 : 270 * 64;

    XSetForeground(m_display, m_graphicsContext, pixel(color));
    XFillArc(m_display, m_window, m_graphicsContext,
             x + m_originX, y + m_originY,
             diameter, diameter, startAngle, 180 * 64);
}

void Painter::drawHalfCircle(int x, int y,
                             unsigned int diameter,
                             bool leftHalf,
                             const Colors& color)
{
    if (!m_display || !m_window || !m_graphicsContext || !diameter)
        return;

    const int startAngle = leftHalf ? 90 * 64 : 270 * 64;

    XSetForeground(m_display, m_graphicsContext, pixel(color));
    XDrawArc(m_display, m_window, m_graphicsContext,
             x + m_originX, y + m_originY,
             diameter - 1, diameter - 1,
             startAngle, 180 * 64);
}

void Painter::drawText(int x, int y,
                       const char* text,
                       const Colors& color)
{
    if (!m_display || !m_window || !text || !m_font)
        return;

    XftDraw* draw = XftDrawCreate(
        m_display, m_window, m_visual, m_colormap);

    if (!draw)
        return;

    XRenderColor renderColor;
    renderColor.red = static_cast<unsigned short>(color.red() * 257);
    renderColor.green = static_cast<unsigned short>(color.green() * 257);
    renderColor.blue = static_cast<unsigned short>(color.blue() * 257);
    renderColor.alpha = static_cast<unsigned short>(color.alpha() * 257);

    XftColor xftColor;

    if (XftColorAllocValue(
            m_display, m_visual, m_colormap,
            &renderColor, &xftColor)) {
        XftDrawStringUtf8(
            draw,
            &xftColor,
            m_font,
            x + m_originX,
            y + m_originY,
            reinterpret_cast<const FcChar8*>(text),
            static_cast<int>(std::strlen(text)));

        XftColorFree(
            m_display, m_visual, m_colormap, &xftColor);
    }

    XftDrawDestroy(draw);
}

int Painter::textWidth(const char* text) const
{
    if (!text || !m_font || !m_display)
        return 0;

    XGlyphInfo extents;

    XftTextExtentsUtf8(
        m_display,
        m_font,
        reinterpret_cast<const FcChar8*>(text),
        static_cast<int>(std::strlen(text)),
        &extents);

    return static_cast<int>(extents.xOff);
}

int Painter::fontAscent() const
{
    return m_font ? m_font->ascent : 0;
}

int Painter::fontDescent() const
{
    return m_font ? m_font->descent : 0;
}

bool Painter::setFont(const std::string& font)
{
    if (!m_display)
        return false;

    XftFont* newFont = XftFontOpenName(
        m_display, DefaultScreen(m_display), font.c_str());

    if (!newFont)
        return false;

    resetFont();
    m_font = newFont;
    m_fontName = font;

    return true;
}

void Painter::resetFont()
{
    if (m_font && m_display)
        XftFontClose(m_display, m_font);

    m_font = nullptr;
    m_fontName.clear();
}

const std::string& Painter::font() const
{
    return m_fontName;
}

unsigned long Painter::pixel(const Colors& color)
{
    return (static_cast<unsigned long>(color.red()) << 16) |
           (static_cast<unsigned long>(color.green()) << 8) |
           static_cast<unsigned long>(color.blue());
}

} // namespace HbTK
