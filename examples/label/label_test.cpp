#include <X11/Xlib.h>

#include <Colors.hpp>
#include <Label.hpp>
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

    window.setTitle("HbTK Label Test");

    if (!window.create(display))
    {
        XCloseDisplay(display);
        return 1;
    }

    HbTK::Label label;

    label.setGeometry(
        20,
        60,
        660,
        180
        );

    label.setText("HbTK Label Test");

    label.setAlignment(
        HbTK::Label::Alignment::Center
        );

    label.setColor(
        HbTK::Colors(40, 120, 220)
        );

    label.setFont("Hack-24");

    window.setCentralWidget(&label);
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