#include <FileTree.hpp>
#include <Colors.hpp>
#include <Painter.hpp>

#include <X11/keysym.h>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>

namespace HbTK {
namespace {

std::string canonicalPath(const std::string& path)
{
    char resolved[PATH_MAX];
    return realpath(path.c_str(), resolved) ? std::string(resolved) : std::string();
}

bool directoryExists(const std::string& path)
{
    struct stat info;
    return stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
}

std::string nameFromPath(const std::string& path)
{
    if (path == "/")
        return path;
    const std::string::size_type slash = path.find_last_of('/');
    return slash == std::string::npos ? path : path.substr(slash + 1);
}

std::string joinPath(const std::string& parent, const std::string& name)
{
    return parent == "/" ? parent + name : parent + "/" + name;
}

} // namespace

FileTree::Node::Node(const std::string& nodeName,
                     const std::string& nodePath,
                     bool isDirectory,
                     Node* nodeParent)
    : name(nodeName),
      path(nodePath),
      directory(isDirectory),
      expanded(false),
      loaded(false),
      parent(nodeParent)
{
}

FileTree::FileTree()
    : m_rowHeight(22),
      m_scrollOffset(0),
      m_indentWidth(18),
      m_scrollbarWidth(12),
      m_hoverRow(-1),
      m_showHiddenFiles(false)
{
    const char* home = std::getenv("HOME");
    setRootPath(home && *home ? home : "/");
}

FileTree::~FileTree() = default;

bool FileTree::setRootPath(const std::string& path)
{
    const std::string resolved = canonicalPath(path);
    if (resolved.empty() || !directoryExists(resolved))
        return false;

    m_rootPath = resolved;
    m_root.reset(new Node(nameFromPath(resolved), resolved, true, nullptr));
    m_root->expanded = true;
    loadChildren(m_root.get());
    m_selectedPath = resolved;
    m_scrollOffset = 0;
    rebuildVisibleRows();
    return true;
}

const std::string& FileTree::rootPath() const
{
    return m_rootPath;
}

bool FileTree::setShowHiddenFiles(bool show)
{
    if (m_showHiddenFiles == show)
        return true;
    m_showHiddenFiles = show;
    refresh();
    return true;
}

bool FileTree::showHiddenFiles() const
{
    return m_showHiddenFiles;
}

const std::string& FileTree::selectedPath() const
{
    return m_selectedPath;
}

bool FileTree::selectPath(const std::string& path)
{
    const std::string resolved = canonicalPath(path);
    if (resolved.empty() || !m_root)
        return false;

    if (resolved != m_rootPath) {
        if (resolved.compare(0, m_rootPath.size(), m_rootPath) != 0 ||
            (m_rootPath != "/" && resolved.size() > m_rootPath.size() &&
             resolved[m_rootPath.size()] != '/'))
            return false;
    }

    std::string remainder = resolved.substr(m_rootPath == "/" ? 1 : m_rootPath.size());
    Node* current = m_root.get();
    std::string::size_type start = 0;
    while (start < remainder.size()) {
        const std::string::size_type end = remainder.find('/', start);
        const std::string part = remainder.substr(
            start, end == std::string::npos ? std::string::npos : end - start);
        if (part.empty())
            break;
        current->expanded = true;
        loadChildren(current);
        Node* next = nullptr;
        for (const auto& child : current->children) {
            if (child->name == part) {
                next = child.get();
                break;
            }
        }
        if (!next)
            return false;
        current = next;
        if (end == std::string::npos)
            break;
        start = end + 1;
    }

    m_selectedPath = current->path;
    rebuildVisibleRows();
    ensureSelectionVisible();
    return true;
}

void FileTree::refresh()
{
    if (!m_root)
        return;
    const std::string selected = m_selectedPath;
    std::function<void(Node*)> clearLoaded = [&](Node* node) {
        for (auto& child : node->children)
            clearLoaded(child.get());
        node->children.clear();
        node->loaded = false;
    };
    clearLoaded(m_root.get());
    loadChildren(m_root.get());
    rebuildVisibleRows();
    if (!selected.empty())
        selectPath(selected);
}

void FileTree::setFileActivatedCallback(
    std::function<void(const std::string&)> callback)
{
    m_fileActivated = callback;
}

void FileTree::setDirectoryActivatedCallback(
    std::function<void(const std::string&)> callback)
{
    m_directoryActivated = callback;
}

void FileTree::resize(int width, int height)
{
    Widget::resize(width, height);
    clampScroll();
}

void FileTree::loadChildren(Node* node)
{
    if (!node || !node->directory || node->loaded)
        return;

    DIR* dir = opendir(node->path.c_str());
    if (!dir) {
        node->loaded = true;
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        const char* name = entry->d_name;
        if (std::strcmp(name, ".") == 0 || std::strcmp(name, "..") == 0)
            continue;
        if (!m_showHiddenFiles && name[0] == '.')
            continue;

        const std::string path = joinPath(node->path, name);
        struct stat info;
        if (lstat(path.c_str(), &info) != 0)
            continue;
        node->children.emplace_back(new Node(
            name, path, S_ISDIR(info.st_mode), node));
    }
    closedir(dir);

    std::sort(node->children.begin(), node->children.end(),
        [](const std::unique_ptr<Node>& a, const std::unique_ptr<Node>& b) {
            if (a->directory != b->directory)
                return a->directory > b->directory;
            return a->name < b->name;
        });
    node->loaded = true;
}

void FileTree::rebuildVisibleRows()
{
    m_visibleRows.clear();
    if (m_root)
        appendVisibleRows(m_root.get());
    clampScroll();
}

void FileTree::appendVisibleRows(Node* node)
{
    if (!node)
        return;
    m_visibleRows.push_back(node);
    if (!node->expanded)
        return;
    for (const auto& child : node->children)
        appendVisibleRows(child.get());
}

FileTree::Node* FileTree::findPath(Node* node, const std::string& path)
{
    if (!node)
        return nullptr;
    if (node->path == path)
        return node;
    for (const auto& child : node->children) {
        if (Node* found = findPath(child.get(), path))
            return found;
    }
    return nullptr;
}

void FileTree::activateSelected()
{
    Node* node = findPath(m_root.get(), m_selectedPath);
    if (!node)
        return;
    if (node->directory) {
        if (m_directoryActivated)
            m_directoryActivated(node->path);
    } else if (m_fileActivated) {
        m_fileActivated(node->path);
    }
}

void FileTree::toggleSelected()
{
    Node* node = findPath(m_root.get(), m_selectedPath);
    if (!node || !node->directory)
        return;
    node->expanded = !node->expanded;
    if (node->expanded)
        loadChildren(node);
    rebuildVisibleRows();
    ensureSelectionVisible();
}

void FileTree::ensureSelectionVisible()
{
    for (std::size_t i = 0; i < m_visibleRows.size(); ++i) {
        if (m_visibleRows[i]->path != m_selectedPath)
            continue;
        const int top = static_cast<int>(i) * m_rowHeight;
        if (top < m_scrollOffset)
            m_scrollOffset = top;
        else if (top + m_rowHeight > m_scrollOffset + height())
            m_scrollOffset = top + m_rowHeight - height();
        break;
    }
    clampScroll();
}

int FileTree::rowAt(int y) const
{
    if (y < 0 || y >= height())
        return -1;
    const int row = (y + m_scrollOffset) / m_rowHeight;
    return row >= 0 && row < static_cast<int>(m_visibleRows.size()) ? row : -1;
}

int FileTree::maximumScroll() const
{
    return std::max(0, static_cast<int>(m_visibleRows.size()) * m_rowHeight - height());
}

void FileTree::clampScroll()
{
    m_scrollOffset = std::max(0, std::min(m_scrollOffset, maximumScroll()));
}

void FileTree::selectRow(int row)
{
    if (row < 0 || row >= static_cast<int>(m_visibleRows.size()))
        return;
    m_selectedPath = m_visibleRows[static_cast<std::size_t>(row)]->path;
    ensureSelectionVisible();
}

void FileTree::paint(Painter& painter)
{
    const Colors background("#202020");
    const Colors foreground("#dddddd");
    const Colors muted("#999999");
    const Colors selected("#365f91");
    const Colors hover("#303840");
    const Colors border("#454545");
    const Colors folder("#e0bd72");
    const Colors file("#bfc7d5");

    painter.fillRect(0, 0, width(), height(), background);
    if (width() > 1 && height() > 1)
        painter.drawRect(0, 0, width() - 1, height() - 1, border);

    const int contentWidth = std::max(0, width() - m_scrollbarWidth);
    const int first = m_scrollOffset / m_rowHeight;
    const int yOffset = -(m_scrollOffset % m_rowHeight);
    for (int row = first, y = yOffset;
         row < static_cast<int>(m_visibleRows.size()) && y < height();
         ++row, y += m_rowHeight) {
        Node* node = m_visibleRows[static_cast<std::size_t>(row)];
        if (node->path == m_selectedPath)
            painter.fillRect(1, y, contentWidth > 1 ? contentWidth - 1 : 0,
                             m_rowHeight, selected);
        else if (row == m_hoverRow)
            painter.fillRect(1, y, contentWidth > 1 ? contentWidth - 1 : 0,
                             m_rowHeight, hover);

        int depth = 0;
        for (Node* parent = node->parent; parent; parent = parent->parent)
            ++depth;
        const int indent = 7 + depth * m_indentWidth;
        if (node->directory) {
            painter.drawText(indent, y + 15, node->expanded ? "v" : ">", muted);
            painter.drawText(indent + 13, y + 15, "[D]", folder);
            painter.drawText(indent + 42, y + 15, node->name.c_str(), foreground);
        } else {
            painter.drawText(indent + 13, y + 15, "[F]", file);
            painter.drawText(indent + 42, y + 15, node->name.c_str(), foreground);
        }
    }

    if (maximumScroll() > 0 && height() > 2) {
        const int trackX = width() - m_scrollbarWidth;
        painter.fillRect(trackX, 1, m_scrollbarWidth - 1, height() - 2,
                         Colors("#292929"));
        const int total = static_cast<int>(m_visibleRows.size()) * m_rowHeight;
        const int thumbHeight = std::max(20, height() * height() / std::max(1, total));
        const int travel = std::max(0, height() - thumbHeight);
        const int thumbY = travel * m_scrollOffset / std::max(1, maximumScroll());
        painter.fillRect(trackX + 2, thumbY + 2, m_scrollbarWidth - 5,
                         std::max(1, thumbHeight - 4), Colors("#777777"));
    }
}

void FileTree::mousePress(int x, int y, unsigned int button)
{
    if (button == 4) {
        m_scrollOffset -= m_rowHeight * 3;
        clampScroll();
        return;
    }
    if (button == 5) {
        m_scrollOffset += m_rowHeight * 3;
        clampScroll();
        return;
    }
    if (button != 1)
        return;

    const int row = rowAt(y);
    if (row < 0)
        return;

    Node* node = m_visibleRows[static_cast<std::size_t>(row)];
    int depth = 0;
    for (Node* parent = node->parent; parent; parent = parent->parent)
        ++depth;
    const int markerStart = 7 + depth * m_indentWidth;
    const bool wasSelected = m_selectedPath == node->path;
    m_selectedPath = node->path;

    if (node->directory && x >= markerStart - 5 && x < markerStart + 12) {
        toggleSelected();
    } else if (node->directory && wasSelected) {
        // A second click on the selected directory activates it.
        activateSelected();
    } else if (!node->directory && wasSelected) {
        activateSelected();
    }
}

void FileTree::mouseRelease(int, int, unsigned int)
{
}

void FileTree::mouseMove(int, int y)
{
    m_hoverRow = rowAt(y);
}

void FileTree::keyPress(unsigned int key)
{
    int row = -1;
    for (std::size_t i = 0; i < m_visibleRows.size(); ++i) {
        if (m_visibleRows[i]->path == m_selectedPath) {
            row = static_cast<int>(i);
            break;
        }
    }

    switch (key) {
    case XK_Up:
        selectRow(row > 0 ? row - 1 : 0);
        break;
    case XK_Down:
        selectRow(row + 1);
        break;
    case XK_Left: {
        Node* node = findPath(m_root.get(), m_selectedPath);
        if (node && node->directory && node->expanded)
            toggleSelected();
        else if (node && node->parent)
            m_selectedPath = node->parent->path;
        ensureSelectionVisible();
        break;
    }
    case XK_Right: {
        Node* node = findPath(m_root.get(), m_selectedPath);
        if (node && node->directory) {
            if (!node->expanded) {
                node->expanded = true;
                loadChildren(node);
                rebuildVisibleRows();
            } else if (!node->children.empty()) {
                m_selectedPath = node->children.front()->path;
            }
            ensureSelectionVisible();
        }
        break;
    }
    case XK_Return:
    case XK_KP_Enter:
        activateSelected();
        break;
    case XK_space:
        toggleSelected();
        break;
    case XK_Page_Up:
        m_scrollOffset -= height();
        clampScroll();
        selectRow(std::max(0, m_scrollOffset / m_rowHeight));
        break;
    case XK_Page_Down:
        m_scrollOffset += height();
        clampScroll();
        selectRow(std::min(static_cast<int>(m_visibleRows.size()) - 1,
                           m_scrollOffset / m_rowHeight));
        break;
    default:
        break;
    }
}

} // namespace HbTK
