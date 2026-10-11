
#ifndef HBTK_OPENFILEDIALOG_HPP
#define HBTK_OPENFILEDIALOG_HPP

#include <X11/Xlib.h>
#include <functional>
#include <string>

namespace HbTK {

class OpenFileDialog
{
public:
    enum class Mode {
        OpenFile,
        SelectDirectory
    };

    OpenFileDialog(int x, int y, int width, int height);
    ~OpenFileDialog();

    OpenFileDialog(const OpenFileDialog&) = delete;
    OpenFileDialog& operator=(const OpenFileDialog&) = delete;

    void setMode(Mode mode);
    void setStartPath(const std::string& path);
    void setAcceptedCallback(
        std::function<void(const std::string&)> callback);
    void setCancelledCallback(std::function<void()> callback);

    bool initialize(Display* display);
    Window window() const;
    bool handleDialogEvent(XEvent& event);

    bool accepted() const;
    const std::string& selectedPath() const;

private:
    class Impl;
    Impl* m_impl;
};

}

#endif
