
#pragma once

#include <string>

#include <X11/Xlib.h>
#include <Widget.hpp>

namespace HbTK {

class Style;
class Application;
class Painter;
class MenuBar;

class Window : public Widget
{
public:
    Window(int x, int y, int width, int height);
    ~Window() override;

    bool create(Display* display);

    void show();
    void hide();

    void setTitle(const std::string& title);
    void setStyle(const Style& style);

    void setCentralWidget(Widget* widget);
    Widget* centralWidget() const;

    void setMenuBar(MenuBar* menuBar);
    MenuBar* menuBar() const;

    Display* display() const;
    ::Window window() const;

    using Widget::paint;
    void paint();

    void resize(int width, int height) override;

    Atom wmDeleteAtom() const;
    bool handleEvent(const XEvent& event);

private:
    Display* m_display;
    ::Window m_window;
    GC m_graphicsContext;
    Visual* m_visual;
    Colormap m_colormap;
    int m_depth;

    std::string m_title;
    const Style* m_style;

    Widget* m_centralWidget;
    MenuBar* m_menuBar;

    bool m_created;
    Atom m_wmDeleteAtom;

    Pixmap m_backBuffer;
    int m_backBufferWidth;
    int m_backBufferHeight;
};

} // namespace HbTK
