
#include <Application.hpp>
#include <MainWindow.hpp>
#include <WidgetContainer.hpp>
#include <Label.hpp>
#include <Entry.hpp>
#include <Colors.hpp>

int main(int argc, char** argv) {
    HbTK::Application app(argc, argv);

    HbTK::Window window(100, 100, 620, 180);
    window.setTitle("HbTK Entry Styles");

    HbTK::WidgetContainer container;
    container.setGeometry(0, 0, 620, 180);
    container.setBackgroundColor(HbTK::Colors(35, 38, 44));

    HbTK::Label modernLabel("Modern");
    modernLabel.setGeometry(20, 38, 180, 20);

    HbTK::Label retroLabel("Retro");
    retroLabel.setGeometry(220, 38, 180, 20);

    HbTK::Label glassLabel("Glass");
    glassLabel.setGeometry(420, 38, 180, 20);

    HbTK::Entry modern(20, 65, 180, 24);
    modern.setLabel("Name");
    modern.setPlaceholder("Enter name");
    modern.setStyle(HbTK::EntryStyle::Modern);
    modern.setAlignment(HbTK::EntryAlignment::Left);
    modern.setFocus(true);

    HbTK::Entry retro(220, 65, 180, 24);
    retro.setLabel("User");
    retro.setPlaceholder("Enter user");
    retro.setStyle(HbTK::EntryStyle::Retro);
    retro.setAlignment(HbTK::EntryAlignment::Center);

    HbTK::Entry glass(420, 65, 180, 24);
    glass.setLabel("Search");
    glass.setPlaceholder("Enter search");
    glass.setStyle(HbTK::EntryStyle::Glass);
    glass.setAlignment(HbTK::EntryAlignment::Right);

    container.addWidget(&modernLabel);
    container.addWidget(&retroLabel);
    container.addWidget(&glassLabel);
    container.addWidget(&modern);
    container.addWidget(&retro);
    container.addWidget(&glass);

    window.setCentralWidget(&container);

    if (!window.create(app.display()))
        return 1;

    app.addWindow(&window);
    window.show();

    return app.exec();
}