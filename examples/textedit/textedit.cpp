
#include <Application.hpp>
#include <MainWindow.hpp>
#include <TextEdit.hpp>

int main(int argc, char** argv) {
    HbTK::Application app(argc, argv);
    HbTK::Window window(100, 100, 800, 600);
    HbTK::TextEdit editor(8, 8, 784, 564);

    window.setTitle("HbTK - TextEdit Test");
    editor.setText(
        "HbTK TextEdit\n"
        "==============\n\n"
        "Multiline text editor widget test.\n"
        "This window contains only the TextEdit widget.\n\n"
        "The next step is testing keyboard input, selection,\n"
        "cursor movement, and scrolling."
    );
    window.setCentralWidget(&editor);
    app.addWindow(&window);

    return app.exec();
}
