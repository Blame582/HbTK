#include <ScrollBar.hpp>
#include <Painter.hpp>
#include <Colors.hpp>

#include <algorithm>

namespace HbTK {

ScrollBar::ScrollBar(Orientation orientation)
    : m_orientation(orientation),
      m_minimum(0),
      m_maximum(100),
      m_pageStep(10),
      m_value(0),
      m_dragOffset(0),
      m_dragging(false) {
    setSize(orientation == Orientation::Vertical ? 14 : 100,
            orientation == Orientation::Vertical ? 100 : 14);
}

void ScrollBar::setRange(int minimum, int maximum) {
    if (maximum < minimum)
        maximum = minimum;

    m_minimum = minimum;
    m_maximum = maximum;
    updateValue(m_value);
}

void ScrollBar::setPageStep(int step) {
    m_pageStep = std::max(1, step);
}

void ScrollBar::setValue(int value) {
    updateValue(value);
}

int ScrollBar::minimum() const {
    return m_minimum;
}

int ScrollBar::maximum() const {
    return m_maximum;
}

int ScrollBar::pageStep() const {
    return m_pageStep;
}

int ScrollBar::value() const {
    return m_value;
}

void ScrollBar::setOnValueChanged(std::function<void(int)> callback) {
    m_onValueChanged = std::move(callback);
}

int ScrollBar::trackLength() const {
    return m_orientation == Orientation::Vertical ? height() : width();
}

int ScrollBar::thumbLength() const {
    const int length = trackLength();
    const int range = m_maximum - m_minimum;

    if (length <= 0 || range <= 0)
        return length;

    const int total = range + m_pageStep;
    const int thumb = total > 0 ? length * m_pageStep / total : length;

    return std::clamp(thumb, std::min(18, length), length);
}

int ScrollBar::thumbPosition() const {
    const int travel = trackLength() - thumbLength();
    const int range = m_maximum - m_minimum;

    if (travel <= 0 || range <= 0)
        return 0;

    return (m_value - m_minimum) * travel / range;
}

int ScrollBar::valueFromPosition(int position) const {
    const int travel = trackLength() - thumbLength();
    const int range = m_maximum - m_minimum;

    if (travel <= 0 || range <= 0)
        return m_minimum;

    position = std::clamp(position, 0, travel);
    return m_minimum + position * range / travel;
}

void ScrollBar::updateValue(int value) {
    value = std::clamp(value, m_minimum, m_maximum);

    if (value == m_value)
        return;

    m_value = value;

    if (m_onValueChanged)
        m_onValueChanged(m_value);
}

void ScrollBar::paint(Painter& painter) {
    if (!visible())
        return;

    const int length = trackLength();
    const int thumb = thumbLength();
    const int position = thumbPosition();

    painter.fillRect(0, 0, width(), height(), Colors(30, 32, 35));

    if (m_orientation == Orientation::Vertical) {
        painter.fillRect(2, position, width() - 4, thumb,
                         Colors(85, 90, 98));
        painter.drawRect(2, position, width() - 4, thumb,
                         Colors(110, 115, 123));
    } else {
        painter.fillRect(position, 2, thumb, height() - 4,
                         Colors(85, 90, 98));
        painter.drawRect(position, 2, thumb, height() - 4,
                         Colors(110, 115, 123));
    }

    (void)length;
}

void ScrollBar::mousePress(int x, int y, unsigned int button) {
    if (button != 1)
        return;

    const int position = m_orientation == Orientation::Vertical ? y : x;
    const int thumbStart = thumbPosition();
    const int thumbEnd = thumbStart + thumbLength();

    if (position >= thumbStart && position < thumbEnd) {
        m_dragging = true;
        m_dragOffset = position - thumbStart;
        return;
    }

    const int direction = position < thumbStart ? -1 : 1;
    updateValue(m_value + direction * m_pageStep);
}

void ScrollBar::mouseRelease(int, int, unsigned int button) {
    if (button == 1)
        m_dragging = false;
}

void ScrollBar::mouseMove(int x, int y) {
    if (!m_dragging)
        return;

    const int position = (m_orientation == Orientation::Vertical ? y : x)
                       - m_dragOffset;

    updateValue(valueFromPosition(position));
}

}