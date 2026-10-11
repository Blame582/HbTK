
#include <WidgetContainer.hpp>
#include <Painter.hpp>

namespace HbTK {

WidgetContainer::WidgetContainer()
    : m_focusWidget(nullptr),
      m_backgroundColor(255, 255, 255),
      m_hasBackgroundColor(false){
}

WidgetContainer::~WidgetContainer(){
    for (Widget* widget : m_widgets)
        delete widget;
}

void WidgetContainer::addWidget(Widget* widget){
    if (!widget)
        return;

    widget->setParent(this);
    m_widgets.push_back(widget);
}

void WidgetContainer::setBackgroundColor(const Colors& color){
    m_backgroundColor = color;
    m_hasBackgroundColor = true;
}

void WidgetContainer::clearBackgroundColor(){
    m_hasBackgroundColor = false;
}

Widget* WidgetContainer::widgetAt(int x, int y) const{
    for (auto it = m_widgets.rbegin();
         it != m_widgets.rend();
         ++it) {
        Widget* widget = *it;

        if (!widget || !widget->visible())
            continue;

        if (x >= widget->x() &&
            y >= widget->y() &&
            x < widget->x() + widget->width() &&
            y < widget->y() + widget->height()) {
            return widget;
        }
    }

    return nullptr;
}

void WidgetContainer::paint(Painter& painter){
    const int oldOriginX = painter.originX();
    const int oldOriginY = painter.originY();

    if (m_hasBackgroundColor)
        painter.fillRect(0, 0, width(), height(), m_backgroundColor);

    for (Widget* widget : m_widgets) {
        if (!widget || !widget->visible())
            continue;

        painter.setOrigin(
            oldOriginX + widget->x(),
            oldOriginY + widget->y());

        widget->paint(painter);

        painter.setOrigin(oldOriginX, oldOriginY);
    }
}

void WidgetContainer::mousePress(int x, int y, unsigned int button){
    Widget* widget = widgetAt(x, y);

    if (!widget)
        return;

    m_focusWidget = widget;

    widget->mousePress(
        x - widget->x(),
        y - widget->y(),
        button);
}

void WidgetContainer::mouseRelease(int x, int y, unsigned int button){
    Widget* widget = widgetAt(x, y);

    if (!widget)
        return;

    widget->mouseRelease(
        x - widget->x(),
        y - widget->y(),
        button);
}

void WidgetContainer::mouseMove(int x, int y){
    Widget* widget = widgetAt(x, y);

    if (!widget)
        return;

    widget->mouseMove(
        x - widget->x(),
        y - widget->y());
}

void WidgetContainer::keyPress(unsigned int keycode){
    if (m_focusWidget)
        m_focusWidget->keyPress(keycode);
}

void WidgetContainer::keyRelease(unsigned int keycode){
    if (m_focusWidget)
        m_focusWidget->keyRelease(keycode);
}

} // namespace HbTK