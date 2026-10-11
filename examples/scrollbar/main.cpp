#include <Application.hpp>
#include <MainWindow.hpp>
#include <ScrollBar.hpp>
#include <Painter.hpp>
#include <Colors.hpp>
#include <Widget.hpp>

#include <algorithm>
#include <string>

class ScrollBarDemo : public HbTK::Widget {
public:
    ScrollBarDemo()
        : m_scrollbar(HbTK::Orientation::Vertical),
          m_scroll(0),
          m_window(nullptr) {
        m_scrollbar.setRange(0, 1580);
        m_scrollbar.setPageStep(420);
        m_scrollbar.setOnValueChanged([this](int value) {
            m_scroll = value;
            if (m_window)
                m_window->paint();
        });
    }

    void setWindow(HbTK::Window* window) {
        m_window = window;
    }

    void resize(int width, int height) override {
        Widget::resize(width, height);
        m_scrollbar.setGeometry(width - 18, 0, 18, height);
        m_scrollbar.setPageStep(height);
        m_scrollbar.setRange(0, std::max(0, 2000 - height));
    }

    void paint(HbTK::Painter& painter) override {
        painter.fillRect(0, 0, width(), height(),
                         HbTK::Colors(20, 22, 25));

        for (int i = 0; i < 100; ++i) {
            const int y = i * 20 - m_scroll;

            if (y < -20 || y > height())
                continue;

            const std::string text =
                "Item " + std::to_string(i + 1);

            painter.drawText(12, y + 15, text.c_str(),
                             HbTK::Colors(210, 214, 218));
        }

        painter.translate(m_scrollbar.x(), m_scrollbar.y());
        m_scrollbar.paint(painter);
        painter.translate(-m_scrollbar.x(), -m_scrollbar.y());
    }

    void mousePress(int x, int y, unsigned int button) override {
        if (x >= m_scrollbar.x() && x < m_scrollbar.x() + m_scrollbar.width()) {
            m_scrollbar.mousePress(x - m_scrollbar.x(),
                                   y - m_scrollbar.y(), button);
            if (m_window)
                m_window->paint();
        }
    }

    void mouseRelease(int x, int y, unsigned int button) override {
        m_scrollbar.mouseRelease(x - m_scrollbar.x(),
                                 y - m_scrollbar.y(), button);
    }

    void mouseMove(int x, int y) override {
        m_scrollbar.mouseMove(x - m_scrollbar.x(),
                              y - m_scrollbar.y());
        if (m_window)
            m_window->paint();
    }

private:
    HbTK::ScrollBar m_scrollbar;
    int m_scroll;
    HbTK::Window* m_window;
};

int main(int argc, char** argv) {
    HbTK::Application app(argc, argv);

    if (!app.display())
        return 1;

    HbTK::Window window(100, 100, 420, 420);
    ScrollBarDemo demo;

    window.setTitle("HbTK ScrollBar Demo");
    window.setCentralWidget(&demo);
    demo.setWindow(&window);
    app.addWindow(&window);

    return app.exec();
}