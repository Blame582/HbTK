// MainWindow.cpp for HbTK
// Copyright (c) 2026 Kevin Day <blame582@gmail.com>

#include <MainWindow.hpp>

#include <Menu.hpp>
#include <MenuBar.hpp>
#include <Painter.hpp>
#include <Style.hpp>

#include <algorithm>

namespace HbTK {

Window::Window(int x,
               int y,
               int width,
               int height)
    : m_display(nullptr),
      m_window(0),
      m_graphicsContext(0),
      m_title("HbTK"),
      m_style(nullptr),
      m_centralWidget(nullptr),
      m_menuBar(nullptr),
      m_created(false),
      m_wmDeleteAtom(0),
      m_backBuffer(0),
      m_backBufferWidth(0),
      m_backBufferHeight(0)
{
    setGeometry(x, y, width, height);
}

Window::~Window()
{
    if (m_backBuffer && m_display)
        XFreePixmap(
            m_display,
            m_backBuffer
            );

    if (m_graphicsContext && m_display)
        XFreeGC(
            m_display,
            m_graphicsContext
            );

    if (m_window && m_display)
        XDestroyWindow(
            m_display,
            m_window
            );
}

bool Window::create(Display* display)
{
    if (m_created)
        return true;

    if (!display)
        return false;

    m_display = display;

    const int screen =
        DefaultScreen(m_display);

    m_window =
        XCreateSimpleWindow(
            m_display,
            RootWindow(m_display, screen),
            x(),
            y(),
            static_cast<unsigned int>(width()),
            static_cast<unsigned int>(height()),
            0,
            BlackPixel(m_display, screen),
            WhitePixel(m_display, screen)
            );

    if (!m_window)
    {
        m_display = nullptr;
        return false;
    }

    const long eventMask =
        ExposureMask |
        StructureNotifyMask |
        ButtonPressMask |
        ButtonReleaseMask |
        PointerMotionMask |
        KeyPressMask |
        KeyReleaseMask;

    XSelectInput(
        m_display,
        m_window,
        eventMask
        );

    m_wmDeleteAtom =
        XInternAtom(
            m_display,
            "WM_DELETE_WINDOW",
            False
            );

    XSetWMProtocols(
        m_display,
        m_window,
        &m_wmDeleteAtom,
        1
        );

    m_graphicsContext =
        XCreateGC(
            m_display,
            m_window,
            0,
            nullptr
            );

    if (!m_graphicsContext)
    {
        XDestroyWindow(
            m_display,
            m_window
            );

        m_window = 0;
        m_display = nullptr;

        return false;
    }

    m_backBuffer =
        XCreatePixmap(
            m_display,
            m_window,
            static_cast<unsigned int>(width()),
            static_cast<unsigned int>(height()),
            DefaultDepth(
                m_display,
                screen
                )
            );

    if (!m_backBuffer)
    {
        XFreeGC(
            m_display,
            m_graphicsContext
            );

        m_graphicsContext = 0;

        XDestroyWindow(
            m_display,
            m_window
            );

        m_window = 0;
        m_display = nullptr;

        return false;
    }

    m_backBufferWidth = width();
    m_backBufferHeight = height();

    XStoreName(
        m_display,
        m_window,
        m_title.c_str()
        );

    m_created = true;

    return true;
}

void Window::show()
{
    if (!m_created)
        return;

    XMapWindow(
        m_display,
        m_window
        );

    XFlush(m_display);
}

void Window::hide()
{
    if (!m_created)
        return;

    XUnmapWindow(
        m_display,
        m_window
        );

    XFlush(m_display);
}

void Window::setTitle(const std::string& title)
{
    m_title = title;

    if (!m_created)
        return;

    XStoreName(
        m_display,
        m_window,
        m_title.c_str()
        );

    XFlush(m_display);
}

void Window::setStyle(const Style& style)
{
    m_style = &style;

    if (m_created)
        paint();
}

void Window::setMenuBar(MenuBar* menuBar)
{
    if (m_menuBar == menuBar)
        return;

    if (m_menuBar)
        m_menuBar->setParent(nullptr);

    m_menuBar = menuBar;

    if (m_menuBar)
    {
        m_menuBar->setParent(this);

        int top = 0;

        if (m_style)
            top = m_style->titleHeight();

        m_menuBar->setGeometry(
            0,
            top,
            width(),
            m_menuBar->barHeight()
            );

        m_menuBar->resize(
            width(),
            m_menuBar->barHeight()
            );
    }

    if (m_centralWidget)
    {
        int top = 0;

        if (m_style)
            top = m_style->titleHeight();

        if (m_menuBar)
            top += m_menuBar->barHeight();

        m_centralWidget->setGeometry(
            0,
            top,
            width(),
            std::max(0, height() - top)
            );

        m_centralWidget->resize(
            width(),
            std::max(0, height() - top)
            );
    }

    if (m_created)
        paint();
}

MenuBar* Window::menuBar() const
{
    return m_menuBar;
}

void Window::setCentralWidget(Widget* widget)
{
    if (m_centralWidget == widget)
        return;

    if (m_centralWidget)
        m_centralWidget->setParent(nullptr);

    m_centralWidget = widget;

    if (!m_centralWidget)
    {
        if (m_created)
            paint();

        return;
    }

    m_centralWidget->setParent(this);

    int top = 0;

    if (m_style)
        top = m_style->titleHeight();

    if (m_menuBar)
        top += m_menuBar->barHeight();

    m_centralWidget->setGeometry(
        0,
        top,
        width(),
        std::max(0, height() - top)
        );

    m_centralWidget->resize(
        width(),
        std::max(0, height() - top)
        );

    if (m_created)
        paint();
}

Widget* Window::centralWidget() const
{
    return m_centralWidget;
}

Display* Window::display() const
{
    return m_display;
}

::Window Window::window() const
{
    return m_window;
}

Atom Window::wmDeleteAtom() const
{
    return m_wmDeleteAtom;
}

bool Window::handleEvent(const XEvent& event)
{
    if (!m_created)
        return false;

    if (event.xany.window != m_window)
        return false;

    switch (event.type)
    {
    case Expose:
        if (event.xexpose.count == 0)
            paint();
        break;

    case ConfigureNotify:
        if (event.xconfigure.width != width() ||
            event.xconfigure.height != height())
        {
            resize(
                event.xconfigure.width,
                event.xconfigure.height
                );

            paint();
        }
        break;

    case ButtonPress:
    {
        const int px = event.xbutton.x;
        const int py = event.xbutton.y;

        if (m_menuBar &&
            m_menuBar->visible())
        {
            const int barY =
                m_menuBar->y();

            const int barHeight =
                m_menuBar->barHeight();

            if (py >= barY &&
                py < barY + barHeight)
            {
                m_menuBar->mousePress(
                    px - m_menuBar->x(),
                    py - barY,
                    event.xbutton.button
                    );

                paint();
                break;
            }
        }

        if (m_menuBar)
        {
            for (int i = 0;
                 i < m_menuBar->count();
                 ++i)
            {
                const MenuBar::Entry* entry =
                    m_menuBar->entry(i);

                if (!entry || !entry->menu)
                    continue;

                Menu* menu = entry->menu;

                if (!menu->isShown())
                    continue;

                if (px >= menu->x() &&
                    px < menu->x() + menu->width() &&
                    py >= menu->y() &&
                    py < menu->y() + menu->height())
                {
                    menu->mousePress(
                        px - menu->x(),
                        py - menu->y(),
                        event.xbutton.button
                        );

                    paint();
                    break;
                }
            }
        }
        
        if (m_centralWidget && m_centralWidget->visible()) {
            if (px >= m_centralWidget->x() &&
                px < m_centralWidget->x() + m_centralWidget->width() &&
                py >= m_centralWidget->y() &&
                py < m_centralWidget->y() + m_centralWidget->height()) {
                m_centralWidget->mousePress(
                    px - m_centralWidget->x(),
                    py - m_centralWidget->y(),
                    event.xbutton.button
                );
                paint();
            }
        }

        break;
    }
    
    case ButtonRelease: {
        if (m_centralWidget &&
            m_centralWidget->visible()) {
            m_centralWidget->mouseRelease(
                event.xbutton.x - m_centralWidget->x(),
                event.xbutton.y - m_centralWidget->y(),
                event.xbutton.button
            );
            paint();
        }
        break;
    }

    case MotionNotify:
    {
        const int px = event.xmotion.x;
        const int py = event.xmotion.y;

        if (m_menuBar &&
            m_menuBar->visible())
        {
            const int barY =
                m_menuBar->y();

            const int barHeight =
                m_menuBar->barHeight();

            if (py >= barY &&
                py < barY + barHeight)
            {
                m_menuBar->mouseMove(
                    px - m_menuBar->x(),
                    py - barY
                    );

                paint();
                break;
            }
        }

        if (m_menuBar)
        {
            for (int i = 0;
                 i < m_menuBar->count();
                 ++i)
            {
                const MenuBar::Entry* entry =
                    m_menuBar->entry(i);

                if (!entry || !entry->menu)
                    continue;

                Menu* menu = entry->menu;

                if (!menu->isShown())
                    continue;

                if (px >= menu->x() &&
                    px < menu->x() + menu->width() &&
                    py >= menu->y() &&
                    py < menu->y() + menu->height())
                {
                    menu->mouseMove(
                        px - menu->x(),
                        py - menu->y()
                        );

                    paint();
                    break;
                }
            }
        }
        
        if (m_centralWidget &&
            m_centralWidget->visible()) {
            m_centralWidget->mouseMove(
                px - m_centralWidget->x(),
                py - m_centralWidget->y()
            );
            paint();
        }

        break;
    }

    case ClientMessage:
    {
        if (static_cast<Atom>(
                event.xclient.message_type
                ) ==
            XInternAtom(
                m_display,
                "WM_PROTOCOLS",
                False
                ))
        {
            const Atom protocol =
                static_cast<Atom>(
                    event.xclient.data.l[0]
                    );

            if (protocol == m_wmDeleteAtom)
                return true;
        }

        break;
    }

    default:
        break;
    }

    return false;
}

void Window::resize(int width, int height)
{
    Widget::resize(width, height);

    if (m_created &&
        m_display &&
        (width != m_backBufferWidth ||
         height != m_backBufferHeight))
    {
        if (m_backBuffer)
            XFreePixmap(
                m_display,
                m_backBuffer
                );

        m_backBuffer =
            XCreatePixmap(
                m_display,
                m_window,
                static_cast<unsigned int>(width),
                static_cast<unsigned int>(height),
                DefaultDepth(
                    m_display,
                    DefaultScreen(m_display)
                    )
                );

        m_backBufferWidth = width;
        m_backBufferHeight = height;
    }

    int top = 0;

    if (m_style)
        top = m_style->titleHeight();

    if (m_menuBar)
    {
        m_menuBar->setGeometry(
            0,
            top,
            width,
            m_menuBar->barHeight()
            );

        m_menuBar->resize(
            width,
            m_menuBar->barHeight()
            );

        top += m_menuBar->barHeight();
    }

    if (!m_centralWidget)
        return;

    m_centralWidget->setGeometry(
        0,
        top,
        width,
        std::max(0, height - top)
        );

    m_centralWidget->resize(
        width,
        std::max(0, height - top)
        );
}

void Window::paint()
{
    if (!m_created ||
        !m_backBuffer)
        return;

    Painter painter(
        m_display,
        m_backBuffer,
        m_graphicsContext
        );
        
         painter.setFont("monospace-10");
        
    if (m_style)
    {
        painter.fillRect(
            0,
            0,
            static_cast<unsigned int>(width()),
            static_cast<unsigned int>(height()),
            m_style->windowBackground()
            );

        const int titleHeight =
            m_style->titleHeight();

        if (titleHeight > 0)
        {
            painter.fillRect(
                0,
                0,
                static_cast<unsigned int>(width()),
                static_cast<unsigned int>(titleHeight),
                m_style->windowTitleBackground()
                );

            painter.drawText(
                10,
                titleHeight - 6,
                m_title.c_str(),
                m_style->windowTitleText()
                );
        }
    }
    else
    {
        Colors background(235, 235, 235);
        Colors titleBackground(55, 55, 55);
        Colors titleText(255, 255, 255);

        painter.fillRect(
            0,
            0,
            static_cast<unsigned int>(width()),
            static_cast<unsigned int>(height()),
            background
            );

        painter.fillRect(
            0,
            0,
            static_cast<unsigned int>(width()),
            28,
            titleBackground
            );

        painter.drawText(
            10,
            20,
            m_title.c_str(),
            titleText
            );
    }

    if (m_centralWidget && m_centralWidget->visible())
    {
        painter.translate(m_centralWidget->x(), m_centralWidget->y());
        m_centralWidget->paint(painter);
        painter.translate(-m_centralWidget->x(), -m_centralWidget->y());
    }

    if (m_menuBar &&
        m_menuBar->visible())
    {
        m_menuBar->paint(painter);

        for (int i = 0;
             i < m_menuBar->count();
             ++i)
        {
            const MenuBar::Entry* entry =
                m_menuBar->entry(i);

            if (!entry || !entry->menu)
                continue;

            if (entry->menu->isShown())
                entry->menu->paint(painter);
        }
    }

    /*
     * Copy the completed frame to the X window
     * in one operation.
     */
    XCopyArea(
        m_display,
        m_backBuffer,
        m_window,
        m_graphicsContext,
        0,
        0,
        static_cast<unsigned int>(width()),
        static_cast<unsigned int>(height()),
        0,
        0
        );

    XFlush(m_display);
}

} // namespace HbTK