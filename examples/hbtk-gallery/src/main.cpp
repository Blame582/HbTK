#include <Application.hpp>
#include <MainWindow.hpp>
#include <MenuBar.hpp>
#include <Menu.hpp>
#include <Splitter.hpp>
#include <Widget.hpp>

int main(int argc, char** argv)
{
    HbTK::Application app(argc, argv);

    HbTK::Window window(100, 100, 900, 650);

    window.setTitle("HbTK Gallery");

    HbTK::MenuBar menuBar;

    HbTK::Menu* fileMenu = menuBar.addMenu("File");

    fileMenu->addItem("New");
    fileMenu->addItem("Open");
    fileMenu->addSeparator();
    fileMenu->addItem("Quit");

    HbTK::Menu* editMenu = menuBar.addMenu("Edit");

    editMenu->addItem("Undo");
    editMenu->addItem("Redo");
    editMenu->addSeparator();
    editMenu->addItem("Preferences");

    HbTK::Menu* aboutMenu = menuBar.addMenu("About");

    aboutMenu->addItem("About HbTK");
    aboutMenu->addItem("About Gallery");

    window.setMenuBar(&menuBar);

    HbTK::Splitter splitter(HbTK::Splitter::Orientation::Horizontal);

    HbTK::Widget navigation;
    HbTK::Widget content;

    splitter.addWidget(&navigation);
    splitter.addWidget(&content);

    splitter.setSizes({220, 680});

    splitter.setHandleWidth(4);

    splitter.setPaneFrame(HbTK::Splitter::PaneFrame::Raised);

    splitter.setPaneFrameWidth(2);

    splitter.setPaneBackground(HbTK::Colors(192, 192, 192));

    splitter.setPaneFrameColor(HbTK::Colors(80, 80, 80));

    window.setCentralWidget(&splitter);

    app.addWindow(&window);

    return app.exec();
}