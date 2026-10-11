#include <ToggleSwitch.hpp>

namespace HbTK {

ToggleSwitch::ToggleSwitch()
    : m_checked(false),
      m_style(Modern),
      m_shape(Round) {
    resize(52, 28);
}

void ToggleSwitch::setChecked(bool checked) {
    m_checked = checked;
}

bool ToggleSwitch::isChecked() const {
    return m_checked;
}

void ToggleSwitch::setStyle(Style style) {
    m_style = style;
}

ToggleSwitch::Style ToggleSwitch::switchStyle() const {
    return m_style;
}

void ToggleSwitch::setShape(Shape shape) {
    m_shape = shape;
}

ToggleSwitch::Shape ToggleSwitch::shape() const {
    return m_shape;
}

void ToggleSwitch::paint(Painter& painter) {
    switch (m_style) {
    case Retro:
        paintRetro(painter);
        break;
    case Modern:
        paintModern(painter);
        break;
    case Glass:
        paintGlass(painter);
        break;
    }
}

void ToggleSwitch::mousePress(int x, int y, unsigned int button) {
    if (button != 1 || x < 0 || y < 0 || x >= width() || y >= height())
        return;
    m_checked = !m_checked;
}

void ToggleSwitch::paintRetro(Painter& painter) {
    const int w = width();
    const int h = height();
    const int trackD = h - 6;
    const int trackR = trackD / 2;
    const int trackY = (h - trackD) / 2;
    const int leftX = 3;
    const int rightX = w - trackD - 3;
    const Colors track(45, 45, 45);
    const Colors border(180, 180, 180);
    const int knobD = trackD - 2;
    const int knobY = (h - knobD) / 2;
    const int knobX = m_checked ? rightX + 1 : leftX + 1;
    const Colors knob = m_checked ? Colors(100, 220, 100)
                                  : Colors(210, 210, 210);

    if (m_shape == Square) {
        painter.fillRect(0, 0, w, h, track);
        painter.drawRect(0, 0, w, h, border);
        painter.fillRect(m_checked ? w - h + 3 : 3, 3,
                         h - 6, h - 6, knob);
        painter.drawRect(m_checked ? w - h + 3 : 3, 3,
                         h - 6, h - 6, Colors(20, 20, 20));
        return;
    }

    painter.fillRect(leftX + trackR, trackY,
                     rightX - leftX, trackD, track);
    painter.fillCircle(leftX, trackY, trackD, track);
    painter.fillCircle(rightX, trackY, trackD, track);

    if (m_shape == Round) {
        painter.drawCircle(leftX, trackY, trackD, border);
        painter.drawCircle(rightX, trackY, trackD, border);
    } else {
        painter.fillRect(leftX + trackR, trackY,
                         rightX - leftX, 1, border);
        painter.fillRect(leftX + trackR, trackY + trackD - 1,
                         rightX - leftX, 1, border);
        painter.drawHalfCircle(leftX, trackY, trackD, true, border);
        painter.drawHalfCircle(rightX, trackY, trackD, false, border);
    }

    painter.fillCircle(knobX, knobY, knobD, knob);
    painter.drawCircle(knobX, knobY, knobD, Colors(20, 20, 20));
}

void ToggleSwitch::paintModern(Painter& painter) {
    const int w = width();
    const int h = height();
    const Colors track = m_checked ? Colors(70, 180, 110)
                                   : Colors(90, 94, 100);
    const Colors thumb(245, 245, 245);

    if (m_shape == Square) {
        painter.fillRect(2, 2, w - 4, h - 4, track);
        const int thumbD = h - 8;
        const int thumbX = m_checked ? w - thumbD - 4 : 4;
        painter.fillRect(thumbX, (h - thumbD) / 2, thumbD, thumbD, thumb);
        return;
    }

    const int trackD = h - 6;
    const int trackY = (h - trackD) / 2;
    const int trackR = trackD / 2;
    const int leftX = 3;
    const int rightX = w - trackD - 3;

    painter.fillRect(leftX + trackR, trackY,
                     rightX - leftX, trackD, track);
    painter.fillCircle(leftX, trackY, trackD, track);
    painter.fillCircle(rightX, trackY, trackD, track);

    const int thumbD = trackD - 4;
    const int thumbY = (h - thumbD) / 2;
    const int thumbX = m_checked ? rightX + 2 : leftX + 2;
    painter.fillCircle(thumbX, thumbY, thumbD, thumb);
}

void ToggleSwitch::paintGlass(Painter& painter) {
    const int w = width();
    const int h = height();
    const int trackD = h - 6;
    const int trackY = (h - trackD) / 2;
    const int trackR = trackD / 2;
    const int leftX = 3;
    const int rightX = w - trackD - 3;
    const Colors track(45, 45, 45, 90);
    const Colors knob = m_checked ? Colors(100, 220, 100, 180)
                                  : Colors(210, 210, 210, 160);

    if (m_shape == Square) {
        painter.fillRect(0, 0, w, h, Colors(45, 45, 45, 120));
        const int knobD = h - 6;
        const int knobX = m_checked ? w - knobD - 3 : 3;
        painter.fillRect(knobX, 3, knobD, knobD, knob);
        return;
    }

    
    painter.fillCircle(leftX, trackY, trackD, track);
    painter.fillCircle(rightX, trackY, trackD, track);

    const int knobD = trackD - 2;
    const int knobY = (h - knobD) / 2;
    const int knobX = m_checked ? rightX + 1 : leftX + 1;
    painter.fillCircle(knobX, knobY, knobD, knob);
}

}