
#pragma once

#include <Widget.hpp>
#include <Painter.hpp>

namespace HbTK {

class ToggleSwitch : public Widget {
public:
    enum Style {
        Retro,
        Modern,
        Glass
    };

    enum Shape {
        Square,
        Round,
        Round2
    };

    ToggleSwitch();

    void setChecked(bool checked);
    bool isChecked() const;

    void setStyle(Style style);
    Style switchStyle() const;

    void setShape(Shape shape);
    Shape shape() const;

    void paint(Painter& painter) override;
    void mousePress(int x, int y, unsigned int button) override;

private:
    void paintRetro(Painter& painter);
    void paintModern(Painter& painter);
    void paintGlass(Painter& painter);

    bool m_checked;
    Style m_style;
    Shape m_shape;
};

}
