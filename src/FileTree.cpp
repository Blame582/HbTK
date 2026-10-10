class FileTreeView : public HbTK::Widget {
public:
    explicit FileTreeView(HbTK::FileTree* tree)
        : m_tree(tree), m_treeVisible(true), m_barVisible(true)
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
        if (m_barVisible) {
            painter.fillRect(0, 0, width(), 30, HbTK::Colors("#303030"));
            painter.drawRect(0, 0, width(), 30, HbTK::Colors("#505050"));

            painter.drawText(10, 20, "Open Folder", HbTK::Colors("#ffffff"));
            painter.drawText(120, 20, "Save Project", HbTK::Colors("#ffffff"));
            painter.drawText(245, 20, m_treeVisible ? "Hide Tree" : "Show Tree",
                             HbTK::Colors("#ffffff"));
            painter.drawText(width() - 25, 20, "x", HbTK::Colors("#ffffff"));
        }

        if (m_treeVisible)
            m_tree->paint(painter);
    }

    void mousePress(int x, int y, unsigned int button) override
    {
        if (button != 1)
            return;

        if (m_barVisible && y < 30) {
            if (x >= width() - 35) {
                m_barVisible = false;
                layoutTree();
            } else if (x < 110) {
                std::cout << "Open Folder: folder dialog not implemented yet\n";
            } else if (x < 235) {
                std::cout << "Save Project: project saving not implemented yet\n";
            } else {
                m_treeVisible = !m_treeVisible;
                layoutTree();
            }
            return;
        }

        int treeY = m_barVisible ? y - 30 : y;

        if (m_treeVisible && treeY >= 0)
            m_tree->mousePress(x, treeY, button);
    }

    void mouseRelease(int x, int y, unsigned int button) override
    {
        int treeY = m_barVisible ? y - 30 : y;

        if (m_treeVisible && treeY >= 0)
            m_tree->mouseRelease(x, treeY, button);
    }

    void mouseMove(int x, int y) override
    {
        int treeY = m_barVisible ? y - 30 : y;

        if (m_treeVisible && treeY >= 0)
            m_tree->mouseMove(x, treeY);
    }

    void keyPress(unsigned int key) override
    {
        if (m_treeVisible)
            m_tree->keyPress(key);
    }

private:
    void layoutTree()
    {
        int top = m_barVisible ? 30 : 0;
        m_tree->setGeometry(0, top, width(),
                            m_treeVisible ? height() - top : 0);
    }

    HbTK::FileTree* m_tree;
    bool m_treeVisible;
    bool m_barVisible;
};