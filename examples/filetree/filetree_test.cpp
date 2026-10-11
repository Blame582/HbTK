
#include <X11/Xlib.h>

#include <FileTree.hpp>
#include <MainWindow.hpp>
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
        painter.fillRect(0, 0, width(), 36,
                         HbTK::Colors("#242831ee"));
        painter.fillRect(0, 35, width(), 1,
                         HbTK::Colors("#414957"));

        painter.setFont("Hack:size=11");
        painter.drawText(12, 23, "FILES",
                         HbTK::Colors("#c7d0df"));

        const char* toggleText =
            m_treeVisible ? "Hide tree" : "Show tree";

        painter.drawText(width() - 100, 23, toggleText,
                         HbTK::Colors("#9caec8"));

        if (m_treeVisible) {
            painter.translate(0, 36);
            m_tree->paint(painter);
            painter.translate(0, -36);
        }
    }

    void mousePress(int x, int y, unsigned int button) override
    {
        if (button != Button1)
            return;

        if (y < 36) {
            if (x >= width() - 112) {
                m_treeVisible = !m_treeVisible;
                layoutTree();
            }
            return;
        }

        if (m_treeVisible)
            m_tree->mousePress(x, y - 36, button);
    }

    void mouseRelease(int x, int y, unsigned int button) override
    {
        if (m_treeVisible && y >= 36)
            m_tree->mouseRelease(x, y - 36, button);
    }

    void mouseMove(int x, int y) override
    {
        if (m_treeVisible && y >= 36)
            m_tree->mouseMove(x, y - 36);
    }

    void keyPress(unsigned int key) override
    {
        if (m_treeVisible)
            m_tree->keyPress(key);
    }

private:
    void layoutTree()
    {
        m_tree->setGeometry(
            0, 36, width(),
            m_treeVisible ? height() - 36 : 0);
    }

    HbTK::FileTree* m_tree;
    bool m_treeVisible;
};

int main(int argc, char** argv)
{
    Display* display = XOpenDisplay(nullptr);

    if (!display) {
        std::cerr << "Cannot open X display\n";
        return 1;
    }

    HbTK::Window window(150, 100, 200, 400);
    window.setTitle("HbTK FileTree");

    if (!window.create(display)) {
        std::cerr << "Cannot create HbTK window\n";
        XCloseDisplay(display);
        return 1;
    }

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
        });

    tree.setDirectoryActivatedCallback(
        [](const std::string& path) {
            std::cout << "Directory: " << path << '\n';
        });

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
