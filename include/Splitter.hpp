#pragma once

#include "Widget.hpp"
#include "Colors.hpp"

#include <vector>

namespace HbTK {

class Painter;

class Splitter : public Widget
{
public:
    enum class Orientation
    {
        Horizontal,
        Vertical
    };

    enum class PaneFrame
    {
        NoFrame,
        Flat,
        Raised,
        Sunken
    };

    explicit Splitter(
        Orientation orientation = Orientation::Horizontal
        );

    ~Splitter() override;

    Splitter(const Splitter&) = delete;
    Splitter& operator=(const Splitter&) = delete;

    void setOrientation(Orientation orientation);
    Orientation orientation() const;

    void addWidget(Widget* widget);
    void removeWidget(Widget* widget);

    int count() const;
    Widget* widget(int index) const;

    void setSizes(const std::vector<int>& sizes);
    const std::vector<int>& sizes() const;

    void setHandleWidth(int width);
    int handleWidth() const;

    void setPaneFrame(PaneFrame frame);
    PaneFrame paneFrame() const;

    void setPaneBackground(const Colors& color);
    const Colors& paneBackground() const;

    void setPaneFrameColor(const Colors& color);
    const Colors& paneFrameColor() const;

    void setPaneFrameWidth(int width);
    int paneFrameWidth() const;

    void resize(int width, int height) override;

    void mousePress(int x,
                    int y,
                    unsigned int button) override;

    void mouseRelease(int x,
                      int y,
                      unsigned int button) override;

    void mouseMove(int x,
                   int y) override;

    void paint(Painter& painter) override;

private:
    void updateLayout();
    int handleAt(int x, int y) const;

    Orientation m_orientation;

    std::vector<Widget*> m_widgets;
    std::vector<int> m_sizes;

    int m_handleWidth;

    PaneFrame m_paneFrame;
    Colors m_paneBackground;
    Colors m_paneFrameColor;
    int m_paneFrameWidth;

    int m_activeHandle;
    bool m_dragging;
    int m_dragStart;
    int m_startFirstSize;
    int m_startSecondSize;
};

} // namespace HbTK