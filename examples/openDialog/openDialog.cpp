
#include <X11/Xlib.h>

#include <OpenFileDialog.hpp>
#include <iostream>
#include <string>

int main()
{
    Display* display = XOpenDisplay(nullptr);

    if (!display) {
        std::cerr << "Cannot open X display\n";
        return 1;
    }

    HbTK::OpenFileDialog dialog(200, 120, 560, 460);
    dialog.setMode(HbTK::OpenFileDialog::Mode::OpenFile);
    dialog.setStartPath(".");

    dialog.setAcceptedCallback(
        [](const std::string& path) {
            std::cout << "Selected: " << path << '\n';
        });

    dialog.setCancelledCallback(
        []() {
            std::cout << "Cancelled\n";
        });

    if (!dialog.initialize(display)) {
        std::cerr << "Could not initialize file dialog\n";
        XCloseDisplay(display);
        return 1;
    }

    while (dialog.window() != 0) {
        XEvent event;
        XNextEvent(display, &event);

        if (!dialog.handleDialogEvent(event))
            continue;

        if (!dialog.accepted() && event.type == DestroyNotify)
            break;
    }

    XCloseDisplay(display);
    return 0;
}
