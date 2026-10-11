#pragma once

#include <functional>
#include <algorithm>
#include <Widget.hpp>

namespace HbTK {

enum class Orientation {
    Horizontal,
    Vertical
};

class ScrollBar : public Widget {
public:
    explicit ScrollBar(Orientation orientation = Orientation::Vertical);

    void setRange(int minimum, int maximum);
    void setPageStep(int step);
    void setValue(int value);
    int minimum() const;
    int maximum() const;
    int pageStep() const;
    int value() const;

    void setOnValueChanged(std::function<void(int)> callback);

    void paint(Painter& painter) override;
    void mousePress(int x, int y, unsigned int button) override;
    void mouseRelease(int x, int y, unsigned int button) override;
    void mouseMove(int x, int y) override;

private:
    int trackLength() const;
    int thumbLength() const;
    int thumbPosition() const;
    int valueFromPosition(int position) const;
    void updateValue(int value);

    Orientation m_orientation;
    int m_minimum;
    int m_maximum;
    int m_pageStep;
    int m_value;
    int m_dragOffset;
    bool m_dragging;
    std::function<void(int)> m_onValueChanged;
};

}
