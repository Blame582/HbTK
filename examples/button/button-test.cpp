#include <X11/Xlib.h>

#include <Button.hpp>
#include <Colors.hpp>
#include <Label.hpp>
#include <WidgetContainer.hpp>
#include <MainWindow.hpp>

int main()
{
    Display* display = XOpenDisplay(nullptr);

    if (!display)
        return 1;

    HbTK::Window window(200, 150, 700, 360);
    window.setTitle("HbTK Button Styles");

    if (!window.create(display))
    {
        XCloseDisplay(display);
        return 1;
    }

    HbTK::WidgetContainer* content = new HbTK::WidgetContainer;
    content->setBackgroundColor(HbTK::Colors(30, 33, 39));

    HbTK::Label* heading = new HbTK::Label("Button Styles");
    heading->setGeometry(24, 15, 400, 32);
    heading->setColor(HbTK::Colors(230, 235, 242));
    heading->setFont("Hack-16");
    content->addWidget(heading);

    HbTK::Label* modernLabel = new HbTK::Label("Modern");
    modernLabel->setGeometry(24, 55, 220, 24);
    modernLabel->setColor(HbTK::Colors(190, 200, 215));
    modernLabel->setFont("Hack-14");
    content->addWidget(modernLabel);

    HbTK::Button* modern = new HbTK::Button("Modern Button");
    modern->setGeometry(24, 80, 220, 44);
    modern->setStyle(HbTK::ButtonStyle::Modern);
    modern->setFont("Hack-16");
    content->addWidget(modern);

    HbTK::Label* retroLabel = new HbTK::Label("Retro");
    retroLabel->setGeometry(24, 135, 220, 24);
    retroLabel->setColor(HbTK::Colors(190, 200, 215));
    retroLabel->setFont("Hack-14");
    content->addWidget(retroLabel);

    HbTK::Button* retro = new HbTK::Button("Retro Button");
    retro->setGeometry(24, 160, 220, 44);
    retro->setStyle(HbTK::ButtonStyle::Retro);
    retro->setFont("Hack-16");
    content->addWidget(retro);

    HbTK::Label* glassLabel = new HbTK::Label("Glass");
    glassLabel->setGeometry(24, 215, 220, 24);
    glassLabel->setColor(HbTK::Colors(190, 200, 215));
    glassLabel->setFont("Hack-14");
    content->addWidget(glassLabel);

    HbTK::Button* glass = new HbTK::Button("Glass Button");
    glass->setGeometry(24, 240, 220, 44);
    glass->setStyle(HbTK::ButtonStyle::Glass);
    glass->setFont("Hack-16");
    content->addWidget(glass);

    window.setCentralWidget(content);
    window.show();

    while (true)
    {
        XEvent event;
        XNextEvent(display, &event);

        if (window.handleEvent(event))
            break;
    }

    XCloseDisplay(display);
    return 0;
}