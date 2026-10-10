// include/HbTK/Application.hpp

#pragma once

#include <vector>

#include <X11/Xlib.h>

namespace HbTK {

class Window;

class Application
{
public:
    Application(int& argc,
                char** argv);

    ~Application();

    int exec();
    void quit();

    void addWindow(Window* window);

    Display* display() const;

private:
    Display* m_display;
    bool m_running;

    std::vector<Window*> m_windows;
};

} // namespace HbTK