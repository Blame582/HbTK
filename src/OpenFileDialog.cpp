#include <OpenFileDialog.hpp>
#include <Button.hpp>
#include <Entry.hpp>
#include <FileTree.hpp>
#include <Painter.hpp>
#include <Colors.hpp>
#include <MainWindow.hpp>
#include <Widget.hpp>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <string>
#include <vector>

namespace HbTK {

class OpenFileDialog::Impl {
public:
    class Contents : public Widget {
    public:
        explicit Contents(Mode mode)
            : m_mode(mode), m_tree(), m_path(0, 0, 400, 28), m_name(0, 0, 400, 28),
              m_back("Back"), m_up("Up"), m_settings("Settings"),
              m_open(mode == Mode::OpenFile ? "Open" : "Select"), m_cancel("Cancel"),
              m_directory("."), m_result(false), m_focus(0) {
            m_tree.setShowHiddenFiles(false);
            m_path.setText(m_directory);
            m_path.setPlaceholder("Directory path");
            m_name.setPlaceholder("File name");
            m_back.setColor(Colors(55, 55, 55));
            m_up.setColor(Colors(55, 55, 55));
            m_settings.setColor(Colors(55, 55, 55));
            m_open.setColor(Colors(55, 85, 65));
            m_cancel.setColor(Colors(65, 55, 55));
            navigate(m_directory);
            m_tree.setDirectoryActivatedCallback([this](const std::string& path) {
                navigate(path);
            });
            m_tree.setFileActivatedCallback([this](const std::string& path) {
                if (m_mode == Mode::OpenFile) {
                    const std::size_t slash = path.find_last_of('/');
                    m_name.setText(path.substr(slash == std::string::npos ? 0 : slash + 1));
                    acceptPath(path);
                }
            });
        }

        void setMode(Mode mode) {
            m_mode = mode;
            m_open.setText(mode == Mode::OpenFile ? "Open" : "Select");
        }

        void setDirectory(const std::string& path) {
            navigate(path);
        }

        bool result() const {
            return m_result;
        }

        bool done() const {
            return m_done;
        }

        const std::string& selectedPath() const {
            return m_selected;
        }

        void cancel() {
            m_result = false;
            m_done = true;
        }

        void resize(int width, int height) override {
            Widget::resize(width, height);
            layout();
        }

        void paint(Painter& painter) override {
            painter.fillRect(0, 0, width(), height(), Colors(35, 35, 35));
            painter.translate(m_path.x(), m_path.y());
            m_path.paint(painter);
            painter.translate(-m_path.x(), -m_path.y());
            painter.translate(m_back.x(), m_back.y());
            m_back.paint(painter);
            painter.translate(-m_back.x(), -m_back.y());
            painter.translate(m_up.x(), m_up.y());
            m_up.paint(painter);
            painter.translate(-m_up.x(), -m_up.y());
            painter.translate(m_settings.x(), m_settings.y());
            m_settings.paint(painter);
            painter.translate(-m_settings.x(), -m_settings.y());
            painter.fillRect(0, 38, width(), height() - 86, Colors(25, 25, 25));
            painter.translate(m_tree.x(), m_tree.y());
            m_tree.paint(painter);
            painter.translate(-m_tree.x(), -m_tree.y());
            paintFiles(painter);
            painter.translate(m_name.x(), m_name.y());
            m_name.paint(painter);
            painter.translate(-m_name.x(), -m_name.y());
            painter.translate(m_open.x(), m_open.y());
            m_open.paint(painter);
            painter.translate(-m_open.x(), -m_open.y());
            painter.translate(m_cancel.x(), m_cancel.y());
            m_cancel.paint(painter);
            painter.translate(-m_cancel.x(), -m_cancel.y());
        }

        void mousePress(int x, int y, unsigned int button) override {
            if (button != Button1)
                return;

            if (y < 34) {
                if (x < width() - 270) {
                    m_path.mousePress(x, y, button);
                    m_focus = 1;
                } else if (x < width() - 180) {
                    goBack();
                } else if (x < width() - 90) {
                    goUp();
                } else {
                    m_settingsMenu = !m_settingsMenu;
                }
                return;
            }

            if (y >= height() - 42) {
                if (x < width() - 206) {
                    m_name.mousePress(x, y, button);
                    m_focus = 2;
                } else if (x < width() - 100) {
                    accept();
                } else {
                    cancel();
                }
                return;
            }

            if (x < m_treeWidth) {
                m_tree.mousePress(x, y - 38, button);
                const std::string path = m_tree.selectedPath();
                if (!path.empty() && isDirectory(path))
                    navigate(path);
            } else {
                selectFileAt(x - m_treeWidth, y - 38);
            }
        }

        void mouseRelease(int x, int y, unsigned int button) override {
            m_tree.mouseRelease(x, y - 38, button);
            m_back.mouseRelease(x, y, button);
            m_up.mouseRelease(x, y, button);
            m_settings.mouseRelease(x, y, button);
            m_open.mouseRelease(x, y, button);
            m_cancel.mouseRelease(x, y, button);
        }

        void mouseMove(int x, int y) override {
            if (y < 34) {
                m_back.mouseMove(x, y);
                m_up.mouseMove(x, y);
                m_settings.mouseMove(x, y);
            }
        }

        void keyPress(unsigned int key) override {
            if (m_focus == 1) {
                if (key == XK_Return || key == XK_KP_Enter)
                    navigate(m_path.text());
                else
                    m_path.keyPress(key);
            } else if (m_focus == 2) {
                if (key == XK_Return || key == XK_KP_Enter)
                    accept();
                else
                    m_name.keyPress(key);
            } else {
                m_tree.keyPress(key);
            }
        }

    private:
        struct Item {
            std::string name;
            std::string path;
            bool directory;
        };

        Mode m_mode;
        FileTree m_tree;
        Entry m_path, m_name;
        Button m_back, m_up, m_settings, m_open, m_cancel;
        std::string m_directory, m_selected;
        std::vector<std::string> m_history;
        std::vector<Item> m_items;
        bool m_result;
        bool m_done = false;
        bool m_settingsMenu = false;
        int m_focus, m_treeWidth = 240, m_rowHeight = 24, m_scroll = 0;

        static bool isDirectory(const std::string& path) {
            struct stat info {};
            return stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
        }

        void layout() {
            const int toolbar = 34, bottom = 42, buttonWidth = 62;
            m_path.setGeometry(6, 3, std::max(40, width() - 282), 28);
            m_back.setGeometry(width() - 270, 3, 84, 28);
            m_up.setGeometry(width() - 180, 3, 84, 28);
            m_settings.setGeometry(width() - 90, 3, 84, 28);
            m_treeWidth = std::max(160, width() / 3);
            m_tree.setGeometry(0, toolbar, m_treeWidth, std::max(0, height() - toolbar - bottom));
            m_name.setGeometry(6, height() - 36, std::max(40, width() - 206), 28);
            m_open.setGeometry(width() - 194, height() - 36, buttonWidth + 12, 28);
            m_cancel.setGeometry(width() - 112, height() - 36, buttonWidth + 12, 28);
        }

        void navigate(const std::string& path) {
            if (path.empty() || !isDirectory(path))
                return;

            char resolved[4096];
            if (!realpath(path.c_str(), resolved))
                return;

            const std::string next(resolved);
            if (next != m_directory)
                m_history.push_back(m_directory);

            m_directory = next;
            m_path.setText(m_directory);
            m_tree.setRootPath(m_directory);
            m_items.clear();
            m_scroll = 0;

            DIR* dir = opendir(m_directory.c_str());
            if (!dir)
                return;

            while (dirent* entry = readdir(dir)) {
                const std::string name(entry->d_name);
                if (name == "." || name == ".." || name[0] == '.')
                    continue;

                const std::string full = m_directory == "/" ? "/" + name : m_directory + "/" + name;
                m_items.push_back({name, full, isDirectory(full)});
            }
            closedir(dir);

            std::sort(m_items.begin(), m_items.end(), [](const Item& a, const Item& b) {
                if (a.directory != b.directory)
                    return a.directory > b.directory;
                return a.name < b.name;
            });
        }

        void goBack() {
            if (m_history.empty())
                return;

            const std::string previous = m_history.back();
            m_history.pop_back();
            m_directory = previous;
            m_path.setText(m_directory);
            m_tree.setRootPath(m_directory);
            reloadItems();
        }

        void goUp() {
            if (m_directory == "/")
                return;

            const std::size_t slash = m_directory.find_last_of('/');
            navigate(slash == 0 ? "/" : m_directory.substr(0, slash));
        }

        void reloadItems() {
            const std::string current = m_directory;
            m_directory.clear();
            navigate(current);
        }

        void paintFiles(Painter& painter) {
            const int left = m_treeWidth, top = 38, rowHeight = m_rowHeight;
            painter.fillRect(left, top, width() - left, height() - top - 42, Colors(25, 25, 25));

            for (std::size_t i = static_cast<std::size_t>(m_scroll); i < m_items.size(); ++i) {
                const int y = top + static_cast<int>(i - m_scroll) * rowHeight;
                if (y + rowHeight > height() - 42)
                    break;

                const Item& item = m_items[i];
                painter.drawText(left + 10, y + 17, item.directory ? "[DIR] " : "      ", Colors(130, 170, 210));
                painter.drawText(left + 60, y + 17, item.name.c_str(), Colors(225, 225, 225));
            }
        }

        void selectFileAt(int x, int y) {
            (void)x;
            if (y < 0)
                return;

            const int index = y / m_rowHeight + m_scroll;
            if (index < 0 || static_cast<std::size_t>(index) >= m_items.size())
                return;

            const Item& item = m_items[static_cast<std::size_t>(index)];
            if (item.directory) {
                navigate(item.path);
            } else {
                m_name.setText(item.name);
                m_selected = item.path;
            }
        }

        void acceptPath(const std::string& path) {
            m_selected = path;
            m_result = true;
            m_done = true;
        }

        void accept() {
            if (m_mode == Mode::SelectDirectory) {
                acceptPath(m_directory);
                return;
            }

            std::string path = m_name.text();
            if (path.empty())
                path = m_selected;
            else if (path[0] != '/')
                path = m_directory + "/" + path;

            struct stat info {};
            if (!path.empty() && stat(path.c_str(), &info) == 0 && S_ISREG(info.st_mode))
                acceptPath(path);
        }
    };

    Impl(int x, int y, int width, int height)
        : contents(OpenFileDialog::Mode::OpenFile), display(nullptr), win(0),
          posX(x), posY(y), winWidth(width), winHeight(height), notified(false) {
    }

    Contents contents;
    Display* display;
    ::Window win;
    int posX, posY, winWidth, winHeight;
    std::string title = "Open File";
    std::function<void(const std::string&)> acceptedCallback;
    std::function<void()> cancelledCallback;
    bool notified;

    bool initialize(Display* d) {
        if (!d)
            return false;

        display = d;
        const int screen = DefaultScreen(display);
        win = XCreateSimpleWindow(display, RootWindow(display, screen),
                                  posX, posY, winWidth, winHeight, 1,
                                  BlackPixel(display, screen),
                                  WhitePixel(display, screen));
        if (!win)
            return false;

        XStoreName(display, win, title.c_str());

        wmDelete = XInternAtom(display, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(display, win, &wmDelete, 1);

        XSelectInput(display, win, ExposureMask | StructureNotifyMask |
                     ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
                     KeyPressMask);

        contents.resize(winWidth, winHeight);
        XMapWindow(display, win);
        return true;
    }
    
    bool handleDialogEvent(XEvent& event) {
        if (!display || !win)
            return false;

        if (event.xany.window != win)
            return false;

        switch (event.type) {
        case Expose:
            if (event.xexpose.count == 0)
                paint();
            break;
        case ConfigureNotify:
            winWidth = event.xconfigure.width;
            winHeight = event.xconfigure.height;
            contents.resize(winWidth, winHeight);
            paint();
            break;
        case ButtonPress:
            contents.mousePress(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            paint();
            break;
        case ButtonRelease:
            contents.mouseRelease(event.xbutton.x, event.xbutton.y, event.xbutton.button);
            paint();
            break;
        case MotionNotify:
            contents.mouseMove(event.xmotion.x, event.xmotion.y);
            paint();
            break;
        case KeyPress: {
            KeySym key = XLookupKeysym(&event.xkey, 0);
            contents.keyPress(static_cast<unsigned int>(key));
            paint();
            break;
        }
        case DestroyNotify:
            win = 0;
            contents.cancel();
            break;
        case ClientMessage:
            if (static_cast<Atom>(event.xclient.data.l[0]) == wmDelete) {
                XDestroyWindow(display, win);
                win = 0;
                contents.cancel();
            }
            break;
        default:
            break;
        }

        if (contents.done() && !notified) {
            notified = true;
            if (contents.result()) {
                if (acceptedCallback)
                    acceptedCallback(contents.selectedPath());
            } else if (cancelledCallback) {
                cancelledCallback();
            }
        }

        return true;
    }

    void paint() {
        if (!display || !win)
            return;
        GC gc = XCreateGC(display, win, 0, nullptr);
        if (!gc)
            return;
        HbTK::Painter painter(display, win, gc);
        painter.setFont("monospace-10");
        contents.paint(painter);
        XFreeGC(display, gc);
        XFlush(display);
    }

    Atom wmDelete = None;
};

OpenFileDialog::OpenFileDialog(int x, int y, int width, int height)
    : m_impl(new Impl(x, y, width, height)) {
}

OpenFileDialog::~OpenFileDialog() {
    if (m_impl) {
        if (m_impl->display && m_impl->win)
            XDestroyWindow(m_impl->display, m_impl->win);
        delete m_impl;
    }
}

void OpenFileDialog::setMode(Mode mode) {
    m_impl->contents.setMode(mode);
    m_impl->title = mode == Mode::OpenFile ? "Open File" : "Select Folder";
    if (m_impl->display && m_impl->win)
        XStoreName(m_impl->display, m_impl->win, m_impl->title.c_str());
}

void OpenFileDialog::setStartPath(const std::string& path) {
    m_impl->contents.setDirectory(path);
}

void OpenFileDialog::setAcceptedCallback(std::function<void(const std::string&)> callback) {
    m_impl->acceptedCallback = callback;
}

void OpenFileDialog::setCancelledCallback(std::function<void()> callback) {
    m_impl->cancelledCallback = callback;
}

bool OpenFileDialog::initialize(Display* display) {
    return m_impl->initialize(display);
}

::Window OpenFileDialog::window() const {
    return m_impl && !m_impl->contents.done() ? m_impl->win : 0;
}

bool OpenFileDialog::handleDialogEvent(XEvent& event) {
    return m_impl->handleDialogEvent(event);
}

bool OpenFileDialog::accepted() const {
    return m_impl->contents.result();
}

const std::string& OpenFileDialog::selectedPath() const {
    return m_impl->contents.selectedPath();
}

}