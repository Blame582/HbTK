// Application.cpp for HbTK
// Copyright (c) 2026 Kevin Day <blame582@gmail.com>

#include <Application.hpp>
#include <MainWindow.hpp>

#include <algorithm>
#include <sys/select.h>
#include <unistd.h>

namespace HbTK {

Application::Application(int& argc, char** argv)
    : m_display(nullptr),
      m_running(false) {
    (void)argc;
    (void)argv;
    m_display = XOpenDisplay(nullptr);
}

Application::~Application() {
    if (m_display)
        XCloseDisplay(m_display);
}

int Application::exec() {
    if (!m_display)
        return 1;

    m_running = true;
    const int xfd = ConnectionNumber(m_display);

    while (m_running) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(xfd, &readfds);

        struct timeval timeout {};
        timeout.tv_usec = 20000;

        const int result = select(xfd + 1, &readfds, nullptr, nullptr, &timeout);

        if (result > 0 && FD_ISSET(xfd, &readfds)) {
            while (XPending(m_display) > 0) {
                XEvent event;
                XNextEvent(m_display, &event);

                for (Window* window : m_windows) {
                    if (window && window->handleEvent(event)) {
                        quit();
                        break;
                    }
                }
            }
        }
    }

    return 0;
}

void Application::quit() {
    m_running = false;
}

void Application::addWindow(Window* window) {
    if (!window || !m_display)
        return;

    if (!window->create(m_display))
        return;

    m_windows.push_back(window);
    window->show();
}

Display* Application::display() const {
    return m_display;
}

}
