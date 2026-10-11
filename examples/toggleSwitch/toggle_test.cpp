
#include <Application.hpp>
#include <MainWindow.hpp>
#include <WidgetContainer.hpp>
#include <Painter.hpp>
#include <ToggleSwitch.hpp>

#include <string>

namespace {

class TextLabel : public HbTK::Widget {
public:
    explicit TextLabel(const std::string& text)
        : m_text(text)
    {
        setSize(180, 24);
    }

    void paint(HbTK::Painter& painter) override
    {
        painter.drawText(0, 16, m_text.c_str(),
                         HbTK::Colors(210, 210, 210));
    }

private:
    std::string m_text;
};

void addExample(HbTK::WidgetContainer* panel,
                int x, int y,
                const char* name,
                HbTK::ToggleSwitch::Style style,
                HbTK::ToggleSwitch::Shape shape)
{
    TextLabel* label = new TextLabel(name);
    label->setPosition(x, y);
    panel->addWidget(label);

    HbTK::ToggleSwitch* toggle = new HbTK::ToggleSwitch;
    toggle->setPosition(x + 190, y - 6);
    toggle->setStyle(style);
    toggle->setShape(shape);
    panel->addWidget(toggle);
}

}

int main(int argc, char** argv)
{
    HbTK::Application app(argc, argv);

    HbTK::Window window(100, 100, 480, 390);
    window.setTitle("HbTK Toggle Switch Test");

    HbTK::WidgetContainer* panel = new HbTK::WidgetContainer;
    panel->setBackgroundColor(HbTK::Colors(30, 32, 36));
    window.setCentralWidget(panel);

    addExample(panel, 24, 32, "Retro / Square",
               HbTK::ToggleSwitch::Retro,
               HbTK::ToggleSwitch::Square);

    addExample(panel, 24, 76, "Retro / Round",
               HbTK::ToggleSwitch::Retro,
               HbTK::ToggleSwitch::Round);
               
    addExample(panel, 24, 120, "Retro / Round2",
               HbTK::ToggleSwitch::Retro,
               HbTK::ToggleSwitch::Round2);

    addExample(panel, 24, 164, "Modern / Square",
               HbTK::ToggleSwitch::Modern,
               HbTK::ToggleSwitch::Square);

    addExample(panel, 24, 208, "Modern / Round",
               HbTK::ToggleSwitch::Modern,
               HbTK::ToggleSwitch::Round);

    addExample(panel, 24, 296, "Glass / Square",
               HbTK::ToggleSwitch::Glass,
               HbTK::ToggleSwitch::Square);

    addExample(panel, 24, 340, "Glass / Round",
               HbTK::ToggleSwitch::Glass,
               HbTK::ToggleSwitch::Round);

    app.addWindow(&window);
    return app.exec();
}
