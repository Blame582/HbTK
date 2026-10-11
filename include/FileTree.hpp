#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <Widget.hpp>

namespace HbTK {

class Painter;

class FileTree : public Widget
{
public:
    FileTree();
    ~FileTree() override;

    FileTree(const FileTree&) = delete;
    FileTree& operator=(const FileTree&) = delete;

    bool setRootPath(const std::string& path);
    const std::string& rootPath() const;

    bool setShowHiddenFiles(bool show);
    bool showHiddenFiles() const;

    const std::string& selectedPath() const;
    bool selectPath(const std::string& path);
    void refresh();

    void setFileActivatedCallback(
        std::function<void(const std::string&)> callback);
    void setDirectoryActivatedCallback(
        std::function<void(const std::string&)> callback);

    void resize(int width, int height) override;
    void paint(Painter& painter) override;
    void mousePress(int x, int y, unsigned int button) override;
    void mouseRelease(int x, int y, unsigned int button) override;
    void mouseMove(int x, int y) override;
    void keyPress(unsigned int keycode) override;

private:
    struct Node
    {
        std::string name;
        std::string path;
        bool directory;
        bool expanded;
        bool loaded;
        Node* parent;
        std::vector<std::unique_ptr<Node>> children;

        Node(const std::string& nodeName,
             const std::string& nodePath,
             bool isDirectory,
             Node* nodeParent);
    };

    void loadChildren(Node* node);
    void rebuildVisibleRows();
    void appendVisibleRows(Node* node);
    Node* findPath(Node* node, const std::string& path);
    void activateSelected();
    void toggleSelected();
    void ensureSelectionVisible();
    int rowAt(int y) const;
    int maximumScroll() const;
    void clampScroll();
    void selectRow(int row);

    std::unique_ptr<Node> m_root;
    std::vector<Node*> m_visibleRows;
    std::string m_rootPath;
    std::string m_selectedPath;
    std::function<void(const std::string&)> m_fileActivated;
    std::function<void(const std::string&)> m_directoryActivated;
    int m_rowHeight;
    int m_scrollOffset;
    int m_indentWidth;
    int m_scrollbarWidth;
    int m_hoverRow;
    bool m_showHiddenFiles;
};

} // namespace HbTK
