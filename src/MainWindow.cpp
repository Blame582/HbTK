// MainWindow.cpp for HbTK
// Copyright (c) 2026 Kevin Day <blame582@gmail.com>

#include <MainWindow.hpp>
#include <Painter.hpp>
#include <Style.hpp>

#include <algorithm>
#include <X11/keysym.h>
#include <X11/Xutil.h>

namespace HbTK {

Window::Window(int x, int y, int width, int height)
    : m_display(nullptr),
      m_window(0),
      m_graphicsContext(0),
      m_title("HbTK"),
      m_style(nullptr),
      m_centralWidget(nullptr),
      m_created(false),
      m_wmDeleteAtom(0) {
    setGeometry(x, y, width, height);
}

Window::~Window() {
    if (m_graphicsContext && m_display)
        XFreeGC(m_display, m_graphicsContext);

    if (m_window && m_display)
        XDestroyWindow(m_display, m_window);
}

bool Window::create(Display* display) {
    if (m_created)
        return true;

    if (!display)
        return false;

    m_display = display;
    const int screen = DefaultScreen(m_display);

    m_window = XCreateSimpleWindow(
        m_display, RootWindow(m_display, screen),
        x(), y(), static_cast<unsigned int>(width()),
        static_cast<unsigned int>(height()), 0,
        BlackPixel(m_display, screen),
        WhitePixel(m_display, screen));

    if (!m_window) {
        m_display = nullptr;
        return false;
    }

    XSelectInput(m_display, m_window,
        ExposureMask | StructureNotifyMask |
        ButtonPressMask | ButtonReleaseMask |
        PointerMotionMask | KeyPressMask | KeyReleaseMask);

    m_wmDeleteAtom = XInternAtom(m_display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(m_display, m_window, &m_wmDeleteAtom, 1);

    m_graphicsContext = XCreateGC(m_display, m_window, 0, nullptr);

    if (!m_graphicsContext) {
        XDestroyWindow(m_display, m_window);
        m_window = 0;
        m_display = nullptr;
        return false;
    }

    XStoreName(m_display, m_window, m_title.c_str());
    m_created = true;
    return true;
}

void Window::show() {
    if (!m_created)
        return;

    XMapWindow(m_display, m_window);
    XFlush(m_display);

    XWindowAttributes attributes;
    if (XGetWindowAttributes(m_display, m_window, &attributes) &&
        attributes.map_state == IsViewable) {
        XSetInputFocus(m_display, m_window, RevertToPointerRoot, CurrentTime);
        XFlush(m_display);
    }
}

void Window::hide() {
    if (!m_created)
        return;

    XUnmapWindow(m_display, m_window);
    XFlush(m_display);
}

void Window::setTitle(const std::string& title) {
    m_title = title;

    if (!m_created)
        return;

    XStoreName(m_display, m_window, m_title.c_str());
    XFlush(m_display);
}

void Window::setStyle(const Style& style) {
    m_style = &style;

    if (m_created)
        paint();
}

void Window::setCentralWidget(Widget* widget) {
    if (m_centralWidget == widget)
        return;

    if (m_centralWidget)
        m_centralWidget->setParent(nullptr);

    m_centralWidget = widget;

    if (!m_centralWidget) {
        if (m_created)
            paint();
        return;
    }

    m_centralWidget->setParent(this);

    const int top = m_style ? m_style->titleHeight() : 0;
    m_centralWidget->setGeometry(0, top, width(),
        std::max(0, height() - top));

    if (m_created)
        paint();
}

Widget* Window::centralWidget() const {
    return m_centralWidget;
}

Display* Window::display() const {
    return m_display;
}

::Window Window::window() const {
    return m_window;
}

Atom Window::wmDeleteAtom() const {
    return m_wmDeleteAtom;
}

bool Window::handleEvent(const XEvent& event) {
    if (!m_created || event.xany.window != m_window)
        return false;

    switch (event.type) {
        case Expose:
            if (event.xexpose.count == 0)
                paint();
            break;

        case ConfigureNotify:
            if (event.xconfigure.width != width() ||
                event.xconfigure.height != height()) {
                resize(event.xconfigure.width, event.xconfigure.height);
                paint();
            }
            break;

        case KeyPress:
            if (m_centralWidget) {
                char buffer[64] {};
                KeySym symbol = NoSymbol;
                const int length = XLookupString(
                    const_cast<XKeyEvent*>(&event.xkey),
                    buffer, sizeof(buffer), &symbol, nullptr);

                if (length > 0) {
                    for (int i = 0; i < length; ++i)
                        m_centralWidget->keyPress(
                            static_cast<unsigned char>(buffer[i]));
                } else {
                    m_centralWidget->keyPress(
                        static_cast<unsigned int>(symbol));
                }
            }
            break;

        case ClientMessage:
            if (static_cast<Atom>(event.xclient.message_type) ==
                XInternAtom(m_display, "WM_PROTOCOLS", False)) {
                if (static_cast<Atom>(event.xclient.data.l[0]) ==
                    m_wmDeleteAtom)
                    return true;
            }
            break;

        case ButtonPress:
            if (m_centralWidget)
                m_centralWidget->mousePress(
                    event.xbutton.x, event.xbutton.y, event.xbutton.button);
            paint();
            break;

        case MotionNotify:
            if (m_centralWidget && (event.xmotion.state & Button1Mask)) {
                m_centralWidget->mouseMove(
                    event.xmotion.x, event.xmotion.y);
                paint();
            }
            break;

        case ButtonRelease:
            if (m_centralWidget)
                m_centralWidget->mouseRelease(
                    event.xbutton.x, event.xbutton.y, event.xbutton.button);
            paint();
            break;


        default:
            break;
    }

    return false;
}

void Window::resize(int width, int height) {
    Widget::resize(width, height);

    if (!m_centralWidget)
        return;

    const int top = m_style ? m_style->titleHeight() : 0;
    m_centralWidget->setGeometry(0, top, width,
        std::max(0, height - top));
}

void Window::paint() {
    if (!m_created)
        return;

    Painter painter(m_display, m_window, m_graphicsContext);

    if (m_style) {
        painter.fillRect(0, 0, static_cast<unsigned int>(width()),
            static_cast<unsigned int>(height()),
            m_style->windowBackground());

        const int titleHeight = m_style->titleHeight();

        if (titleHeight > 0) {
            painter.fillRect(0, 0, static_cast<unsigned int>(width()),
                static_cast<unsigned int>(titleHeight),
                m_style->windowTitleBackground());

            painter.drawText(10, titleHeight - 6, m_title.c_str(),
                m_style->windowTitleText());
        }
    } else {
        painter.fillRect(0, 0, static_cast<unsigned int>(width()),
            static_cast<unsigned int>(height()), Colors(12, 14, 16));
    }

    if (m_centralWidget && m_centralWidget->visible())
        m_centralWidget->paint(painter);

    XFlush(m_display);
}

}
