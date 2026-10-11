
#include <TerminalWidget.hpp>
#include <Painter.hpp>
#include <Colors.hpp>

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <string>

#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <pty.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#include <X11/keysym.h>

namespace HbTK {

TerminalWidget::TerminalWidget()
    : m_masterFd(-1),
      m_pid(-1),
      m_columns(80),
      m_rows(24),
      m_running(false) {
    m_lines.emplace_back();
}

TerminalWidget::~TerminalWidget() {
    stop();
}

std::string TerminalWidget::shellPath() const {
    if (!m_shell.empty())
        return m_shell;

    const char* shell = std::getenv("SHELL");
    return shell && *shell ? shell : "/bin/sh";
}

bool TerminalWidget::createPty() {
    struct winsize size {};
    size.ws_col = static_cast<unsigned short>(m_columns);
    size.ws_row = static_cast<unsigned short>(m_rows);
    size.ws_xpixel = static_cast<unsigned short>(width());
    size.ws_ypixel = static_cast<unsigned short>(height());

    int master = -1;
    int slave = -1;

    if (openpty(&master, &slave, nullptr, nullptr, &size) == -1)
        return false;

    pid_t pid = fork();

    if (pid == -1) {
        close(master);
        close(slave);
        return false;
    }

    if (pid == 0) {
        close(master);

        if (setsid() == -1)
            _exit(127);

        if (ioctl(slave, TIOCSCTTY, 0) == -1)
            _exit(127);

        if (dup2(slave, STDIN_FILENO) == -1 ||
            dup2(slave, STDOUT_FILENO) == -1 ||
            dup2(slave, STDERR_FILENO) == -1)
            _exit(127);

        if (slave > STDERR_FILENO)
            close(slave);

        const std::string shell = shellPath();
        setenv("TERM", "xterm", 1);

        execl(shell.c_str(), shell.c_str(), static_cast<char*>(nullptr));
        _exit(127);
    }

    close(slave);

    m_masterFd = master;
    m_pid = pid;

    const int flags = fcntl(m_masterFd, F_GETFL, 0);
    if (flags != -1)
        fcntl(m_masterFd, F_SETFL, flags | O_NONBLOCK);

    updatePtySize();
    return true;
}

bool TerminalWidget::start(const std::string& shell) {
    if (m_running)
        return true;

    m_shell = shell;
    m_lines.clear();
    m_lines.emplace_back();
    m_currentLine.clear();

    if (!createPty())
        return false;

    m_running = true;
    return true;
}

void TerminalWidget::stop() {
    m_running = false;

    if (m_masterFd != -1) {
        close(m_masterFd);
        m_masterFd = -1;
    }

    if (m_pid > 0) {
        kill(-m_pid, SIGHUP);
        kill(m_pid, SIGHUP);
        waitpid(m_pid, nullptr, WNOHANG);
        m_pid = -1;
    }
}

bool TerminalWidget::running() const {
    return m_running;
}

pid_t TerminalWidget::processId() const {
    return m_pid;
}

void TerminalWidget::write(const std::string& text) {
    if (!m_running || m_masterFd == -1)
        return;

    const char* data = text.data();
    std::size_t remaining = text.size();

    while (remaining > 0) {
        const ssize_t count = ::write(m_masterFd, data, remaining);

        if (count > 0) {
            data += count;
            remaining -= static_cast<std::size_t>(count);
            continue;
        }

        if (count == -1 && errno == EINTR)
            continue;

        break;
    }
}

void TerminalWidget::resize(int width, int height) {
    Widget::resize(width, height);

    m_columns = std::max(1, width / 8);
    m_rows = std::max(1, height / 16);

    updatePtySize();
}

void TerminalWidget::updatePtySize() {
    if (m_masterFd == -1)
        return;

    struct winsize size {};
    size.ws_col = static_cast<unsigned short>(m_columns);
    size.ws_row = static_cast<unsigned short>(m_rows);
    size.ws_xpixel = static_cast<unsigned short>(width());
    size.ws_ypixel = static_cast<unsigned short>(height());

    ioctl(m_masterFd, TIOCSWINSZ, &size);
}

void TerminalWidget::scrollBuffer() {
    const std::size_t limit = static_cast<std::size_t>(m_rows);

    while (m_lines.size() > limit)
        m_lines.erase(m_lines.begin());
}

void TerminalWidget::appendCharacter(char character) {
    if (character == '\r') {
        m_currentLine.clear();
        return;
    }

    if (character == '\n') {
        m_lines.push_back(m_currentLine);
        m_currentLine.clear();
        scrollBuffer();
        return;
    }

    if (character == '\b' || character == 0x7f) {
        if (!m_currentLine.empty())
            m_currentLine.pop_back();
        return;
    }

    if (character == '\t') {
        const int spaces = 8 - static_cast<int>(m_currentLine.size() % 8);
        m_currentLine.append(static_cast<std::size_t>(spaces), ' ');
        return;
    }

    const unsigned char value = static_cast<unsigned char>(character);

    if (value >= 32 || value >= 0x80)
        m_currentLine += character;

    if (m_currentLine.size() > static_cast<std::size_t>(m_columns))
        m_currentLine.resize(static_cast<std::size_t>(m_columns));
}

void TerminalWidget::readPty() {
    if (!m_running || m_masterFd == -1)
        return;

    char buffer[4096];
    bool changed = false;

    for (;;) {
        const ssize_t count = ::read(m_masterFd, buffer, sizeof(buffer));

        if (count > 0) {
            for (ssize_t i = 0; i < count; ++i) {
                const unsigned char c =
                    static_cast<unsigned char>(buffer[i]);

                if (c == 0x1b) {
                    // ANSI escape parsing will replace this basic renderer.
                    continue;
                }

                if (c < 32 && c != '\n' && c != '\r' &&
                    c != '\b' && c != '\t')
                    continue;

                appendCharacter(static_cast<char>(c));
            }

            changed = true;
            continue;
        }

        if (count == -1 && errno == EINTR)
            continue;

        if (count == -1 &&
            (errno == EAGAIN || errno == EWOULDBLOCK))
            break;

        m_running = false;
        break;
    }

    if (changed)
        scrollBuffer();
}

void TerminalWidget::processOutput() {
    readPty();
}

void TerminalWidget::paint(Painter& painter) {
    if (!visible())
        return;

    painter.fillRect(0, 0,
        static_cast<unsigned int>(width()),
        static_cast<unsigned int>(height()),
        Colors(12, 14, 16));

    const int lineHeight = 16;
    int y = lineHeight;

    for (const auto& line : m_lines) {
        if (y > height())
            break;

        painter.drawText(4, y, line.c_str(), Colors(210, 214, 218));
        y += lineHeight;
    }

    if (y <= height())
        painter.drawText(4, y, m_currentLine.c_str(),
                         Colors(210, 214, 218));
}

void TerminalWidget::keyPress(unsigned int keycode) {
    if (!m_running)
        return;

    switch (keycode) {
        case XK_Return:
        case XK_KP_Enter:
            write("\r");
            return;
        case XK_BackSpace:
            write("\177");
            return;
        case XK_Tab:
            write("\t");
            return;
        case XK_Up:
            write("\033[A");
            return;
        case XK_Down:
            write("\033[B");
            return;
        case XK_Right:
            write("\033[C");
            return;
        case XK_Left:
            write("\033[D");
            return;
        case XK_Home:
            write("\033[H");
            return;
        case XK_End:
            write("\033[F");
            return;
        case XK_Delete:
            write("\033[3~");
            return;
        case XK_Page_Up:
            write("\033[5~");
            return;
        case XK_Page_Down:
            write("\033[6~");
            return;
        default:
            break;
    }

    if (keycode >= 32 && keycode <= 126)
        write(std::string(1, static_cast<char>(keycode)));
}

}
