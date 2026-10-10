#include <X11/Xlib.h>

#include <Button.hpp>
#include <Colors.hpp>
#include <MainWindow.hpp>

int main()
{
    Display* display =
        XOpenDisplay(nullptr);

    if (!display)
        return 1;

    HbTK::Window window(
        200,
        150,
        700,
        300
        );

    window.setTitle("HbTK Button Test");

    if (!window.create(display))
    {
        XCloseDisplay(display);
        return 1;
    }

    HbTK::Button button;

    button.setText("HbTK Button");

    button.setColor(
        HbTK::Colors(192, 192, 192)
        );

    button.setHoverColor(
        HbTK::Colors(210, 210, 210)
        );

    button.setPressedColor(
        HbTK::Colors(160, 160, 160)
        );

    button.setBorderColor(
        HbTK::Colors(64, 64, 64)
        );

    button.setFont("Hack-24");

    window.setCentralWidget(&button);
    
    button.setGeometry(
        200,
        90,
        300,
        100
        );
    
    window.show();

    bool running = true;

    while (running)
    {
        XEvent event;

        XNextEvent(
            display,
            &event
            );

        if (window.handleEvent(event))
            running = false;
    }

    XCloseDisplay(display);

    return 0;
}