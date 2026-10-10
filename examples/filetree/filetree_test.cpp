#include <X11/Xlib.h>

#include <FileTree.hpp>
#include <MainWindow.hpp>
#include <MenuBar.hpp>
#include <Painter.hpp>
#include <Widget.hpp>

#include <iostream>
#include <string>

class FileTreeView : public HbTK::Widget {
public:
    explicit FileTreeView(HbTK::FileTree* tree)
        : m_tree(tree), m_treeVisible(true)
    {
        m_tree->setParent(this);
    }

    void resize(int width, int height) override
    {
        HbTK::Widget::resize(width, height);
        layoutTree();
    }

    void paint(HbTK::Painter& painter) override
    {
        painter.fillRect(0, 0, width(), 30, HbTK::Colors("#303030"));
        painter.drawRect(0, 0, width(), 30, HbTK::Colors("#505050"));

        painter.drawText(10, 20, "Open Folder", HbTK::Colors("#ffffff"));
        painter.drawText(120, 20, "Save Project", HbTK::Colors("#ffffff"));
        painter.drawText(245, 20, m_treeVisible ? "Hide Tree" : "Show Tree",
                         HbTK::Colors("#ffffff"));

        if (m_treeVisible)
            m_tree->paint(painter);
    }

    void mousePress(int x, int y, unsigned int button) override
    {
        if (button != 1)
            return;

        if (y < 30) {
            if (x < 110) {
                std::cout << "Open Folder: folder dialog not implemented yet\n";
            } else if (x < 235) {
                std::cout << "Save Project: project saving not implemented yet\n";
            } else {
                m_treeVisible = !m_treeVisible;
                layoutTree();
            }
            return;
        }

        if (m_treeVisible)
            m_tree->mousePress(x, y - 30, button);
    }

    void mouseRelease(int x, int y, unsigned int button) override
    {
        if (m_treeVisible && y >= 30)
            m_tree->mouseRelease(x, y - 30, button);
    }

    void mouseMove(int x, int y) override
    {
        if (m_treeVisible && y >= 30)
            m_tree->mouseMove(x, y - 30);
    }

    void keyPress(unsigned int key) override
    {
        if (m_treeVisible)
            m_tree->keyPress(key);
    }

private:
    void layoutTree()
    {
        m_tree->setGeometry(0, 30, width(), m_treeVisible ? height() - 30 : 0);
    }

    HbTK::FileTree* m_tree;
    bool m_treeVisible;
};

int main(int argc, char** argv)
{
    Display* display = XOpenDisplay(nullptr);

    if (!display)
        return 1;

    HbTK::Window window(150, 100, 400, 600);
    window.setTitle("HbTK File Tree");

    if (!window.create(display)) {
        XCloseDisplay(display);
        return 1;
    }

    HbTK::MenuBar menuBar;
    HbTK::Menu* fileMenu = menuBar.addMenu("File");
    fileMenu->addItem("Open folder");
    fileMenu->addItem("Open file");
    window.setMenuBar(&menuBar);

    HbTK::FileTree tree;
    const std::string root = argc > 1 ? argv[1] : ".";

    if (!tree.setRootPath(root)) {
        std::cerr << "Cannot open directory: " << root << '\n';
        XCloseDisplay(display);
        return 1;
    }

    tree.setFileActivatedCallback(
        [](const std::string& path) {
            std::cout << "File: " << path << '\n';
        }
    );

    tree.setDirectoryActivatedCallback(
        [](const std::string& path) {
            std::cout << "Directory: " << path << '\n';
        }
    );

    FileTreeView view(&tree);
    window.setCentralWidget(&view);
    window.show();

    bool running = true;

    while (running) {
        XEvent event;
        XNextEvent(display, &event);

        if (window.handleEvent(event))
            running = false;
    }

    XCloseDisplay(display);
    return 0;
}