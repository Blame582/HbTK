#include "Splitter.hpp"
#include "Painter.hpp"

#include <algorithm>

namespace HbTK {

Splitter::Splitter(Orientation orientation)
    : m_orientation(orientation),
      m_handleWidth(4),
      m_paneFrame(PaneFrame::Raised),
      m_paneBackground(Colors(192, 192, 192)),
      m_paneFrameColor(Colors(80, 80, 80)),
      m_paneFrameWidth(2),
      m_activeHandle(-1),
      m_dragging(false),
      m_dragStart(0),
      m_startFirstSize(0),
      m_startSecondSize(0)
{
}

Splitter::~Splitter()
{
}

void Splitter::setOrientation(Orientation orientation)
{
    if (m_orientation == orientation)
        return;

    m_orientation = orientation;
    updateLayout();
}

Splitter::Orientation Splitter::orientation() const
{
    return m_orientation;
}

void Splitter::addWidget(Widget* widget)
{
    if (widget == nullptr)
        return;

    m_widgets.push_back(widget);
    widget->setParent(this);

    m_sizes.clear();

    updateLayout();
}

void Splitter::removeWidget(Widget* widget)
{
    if (widget == nullptr)
        return;

    for (std::size_t i = 0; i < m_widgets.size(); ++i)
    {
        if (m_widgets[i] != widget)
            continue;

        m_widgets.erase(
            m_widgets.begin() + static_cast<long>(i)
        );

        if (i < m_sizes.size())
        {
            m_sizes.erase(
                m_sizes.begin() + static_cast<long>(i)
            );
        }

        widget->setParent(nullptr);

        updateLayout();
        return;
    }
}

int Splitter::count() const
{
    return static_cast<int>(m_widgets.size());
}

Widget* Splitter::widget(int index) const
{
    if (index < 0 || index >= count())
        return nullptr;

    return m_widgets[static_cast<std::size_t>(index)];
}

void Splitter::setSizes(const std::vector<int>& sizes)
{
    if (sizes.size() != m_widgets.size())
        return;

    m_sizes = sizes;
    updateLayout();
}

const std::vector<int>& Splitter::sizes() const
{
    return m_sizes;
}

void Splitter::setHandleWidth(int width)
{
    if (width < 0)
        width = 0;

    if (m_handleWidth == width)
        return;

    m_handleWidth = width;
    updateLayout();
}

int Splitter::handleWidth() const
{
    return m_handleWidth;
}

void Splitter::setPaneFrame(PaneFrame frame)
{
    m_paneFrame = frame;
}

Splitter::PaneFrame Splitter::paneFrame() const
{
    return m_paneFrame;
}

void Splitter::setPaneBackground(const Colors& color)
{
    m_paneBackground = color;
}

const Colors& Splitter::paneBackground() const
{
    return m_paneBackground;
}

void Splitter::setPaneFrameColor(const Colors& color)
{
    m_paneFrameColor = color;
}

const Colors& Splitter::paneFrameColor() const
{
    return m_paneFrameColor;
}

void Splitter::setPaneFrameWidth(int width)
{
    if (width < 0)
        width = 0;

    m_paneFrameWidth = width;
}

int Splitter::paneFrameWidth() const
{
    return m_paneFrameWidth;
}

void Splitter::resize(int width, int height)
{
    Widget::resize(width, height);
    updateLayout();
}

void Splitter::updateLayout()
{
    const int widgetCount = count();

    if (widgetCount == 0)
        return;

    const int total =
        (m_orientation == Orientation::Horizontal)
            ? width()
            : height();

    const int handles =
        m_handleWidth * (widgetCount - 1);

    const int available =
        std::max(0, total - handles);

    /*
     * Make sure we have a size entry for every widget.
     */

    if (m_sizes.size() != m_widgets.size())
    {
        m_sizes.assign(
            static_cast<std::size_t>(widgetCount),
            0
        );
    }

    /*
     * The splitter may receive its requested sizes before
     * it has been given its real geometry.
     *
     * Do not destroy those requested sizes while the
     * available space is zero.
     */

    if (available <= 0)
    {
        int position = 0;

        for (int i = 0; i < widgetCount; ++i)
        {
            Widget* child =
                m_widgets[static_cast<std::size_t>(i)];

            if (child == nullptr)
                continue;

            const int size =
                std::max(
                    0,
                    m_sizes[static_cast<std::size_t>(i)]
                );

            if (m_orientation == Orientation::Horizontal)
            {
                child->setGeometry(
                    x() + position,
                    y(),
                    0,
                    height()
                );
            }
            else
            {
                child->setGeometry(
                    x(),
                    y() + position,
                    width(),
                    0
                );
            }

            position += size;

            if (i < widgetCount - 1)
                position += m_handleWidth;
        }

        return;
    }

    int currentTotal = 0;

    for (int size : m_sizes)
        currentTotal += std::max(0, size);

    /*
     * No requested sizes: divide the available space equally.
     */

    if (currentTotal <= 0)
    {
        const int baseSize =
            available / widgetCount;

        const int remainder =
            available % widgetCount;

        for (int i = 0; i < widgetCount; ++i)
        {
            m_sizes[
                static_cast<std::size_t>(i)
            ] =
                baseSize +
                (i < remainder ? 1 : 0);
        }
    }
    /*
     * Requested sizes exist but do not exactly fit.
     *
     * Preserve the requested proportions while making the
     * total fit the available space.
     */
    else if (currentTotal != available)
    {
        int remaining = available;

        for (int i = 0; i < widgetCount; ++i)
        {
            if (i == widgetCount - 1)
            {
                m_sizes[
                    static_cast<std::size_t>(i)
                ] =
                    std::max(0, remaining);

                break;
            }

            const int size =
                static_cast<int>(
                    (
                        static_cast<long long>(
                            std::max(
                                0,
                                m_sizes[
                                    static_cast<std::size_t>(i)
                                ]
                            )
                        ) *
                        available
                    ) /
                    currentTotal
                );

            m_sizes[
                static_cast<std::size_t>(i)
            ] =
                std::max(0, size);

            remaining -=
                m_sizes[
                    static_cast<std::size_t>(i)
                ];
        }
    }

    /*
     * Apply the calculated sizes to the child widgets.
     */

    int position = 0;

    for (int i = 0; i < widgetCount; ++i)
    {
        Widget* child =
            m_widgets[
                static_cast<std::size_t>(i)
            ];

        if (child == nullptr)
            continue;

        const int size =
            std::max(
                0,
                m_sizes[
                    static_cast<std::size_t>(i)
                ]
            );

        if (m_orientation == Orientation::Horizontal)
        {
            child->setGeometry(
                x() + position,
                y(),
                size,
                height()
            );
        }
        else
        {
            child->setGeometry(
                x(),
                y() + position,
                width(),
                size
            );
        }

        position += size;

        if (i < widgetCount - 1)
            position += m_handleWidth;
    }
}

int Splitter::handleAt(int x, int y) const
{
    const int widgetCount = count();

    if (widgetCount < 2)
        return -1;

    if (m_sizes.size() != m_widgets.size())
        return -1;

    int position = 0;

    for (int i = 0; i < widgetCount - 1; ++i)
    {
        position +=
            std::max(
                0,
                m_sizes[
                    static_cast<std::size_t>(i)
                ]
            );

        if (m_orientation == Orientation::Horizontal)
        {
            if (x >= position &&
                x < position + m_handleWidth)
            {
                return i;
            }
        }
        else
        {
            if (y >= position &&
                y < position + m_handleWidth)
            {
                return i;
            }
        }

        position += m_handleWidth;
    }

    return -1;
}

void Splitter::mousePress(int x,
                          int y,
                          unsigned int button)
{
    if (button != 1)
        return;

    const int handle = handleAt(x, y);

    if (handle < 0)
        return;

    m_activeHandle = handle;
    m_dragging = true;

    m_dragStart =
        (m_orientation == Orientation::Horizontal)
            ? x
            : y;

    m_startFirstSize =
        m_sizes[
            static_cast<std::size_t>(handle)
        ];

    m_startSecondSize =
        m_sizes[
            static_cast<std::size_t>(handle + 1)
        ];
}

void Splitter::mouseRelease(int,
                            int,
                            unsigned int button)
{
    if (button != 1)
        return;

    m_dragging = false;
    m_activeHandle = -1;
}

void Splitter::mouseMove(int x, int y)
{
    if (!m_dragging || m_activeHandle < 0)
        return;

    if (m_sizes.size() != m_widgets.size())
        return;

    const int current =
        (m_orientation == Orientation::Horizontal)
            ? x
            : y;

    const int delta =
        current - m_dragStart;

    constexpr int minimumSize = 32;

    const int total =
        m_startFirstSize +
        m_startSecondSize;

    if (total < minimumSize * 2)
        return;

    int first =
        m_startFirstSize + delta;

    int second =
        m_startSecondSize - delta;

    if (first < minimumSize)
    {
        first = minimumSize;
        second = total - first;
    }

    if (second < minimumSize)
    {
        second = minimumSize;
        first = total - second;
    }

    if (first < minimumSize ||
        second < minimumSize)
    {
        return;
    }

    m_sizes[
        static_cast<std::size_t>(m_activeHandle)
    ] = first;

    m_sizes[
        static_cast<std::size_t>(m_activeHandle + 1)
    ] = second;

    updateLayout();
}

void Splitter::paint(Painter& painter)
{
    if (!visible())
        return;

    /*
     * Paint pane backgrounds first.
     */

    for (std::size_t i = 0;
         i < m_widgets.size();
         ++i)
    {
        Widget* widget = m_widgets[i];

        if (widget == nullptr)
            continue;

        const int paneX = widget->x();
        const int paneY = widget->y();
        const int paneWidth = widget->width();
        const int paneHeight = widget->height();

        if (paneWidth <= 0 || paneHeight <= 0)
            continue;

        painter.fillRect(
            paneX,
            paneY,
            static_cast<unsigned int>(paneWidth),
            static_cast<unsigned int>(paneHeight),
            m_paneBackground
        );
    }

    /*
     * Paint the child widgets.
     */

    for (Widget* widget : m_widgets)
    {
        if (widget && widget->visible())
            widget->paint(painter);
    }

    /*
     * Paint pane frames LAST so they remain visible.
     */

    if (m_paneFrame != PaneFrame::NoFrame &&
        m_paneFrameWidth > 0)
    {
        for (std::size_t i = 0;
             i < m_widgets.size();
             ++i)
        {
            Widget* widget = m_widgets[i];

            if (widget == nullptr)
                continue;

            const int paneX = widget->x();
            const int paneY = widget->y();
            const int paneWidth = widget->width();
            const int paneHeight = widget->height();

            if (paneWidth <= 1 || paneHeight <= 1)
                continue;

            for (int n = 0;
                 n < m_paneFrameWidth;
                 ++n)
            {
                const int left =
                    paneX + n;

                const int top =
                    paneY + n;

                const int right =
                    paneX + paneWidth - 1 - n;

                const int bottom =
                    paneY + paneHeight - 1 - n;

                if (right <= left ||
                    bottom <= top)
                {
                    break;
                }

                Colors topLeft;
                Colors bottomRight;

                switch (m_paneFrame)
                {
                case PaneFrame::Raised:
                    topLeft = Colors(235, 235, 235);
                    bottomRight = Colors(70, 70, 70);
                    break;

                case PaneFrame::Sunken:
                    topLeft = Colors(70, 70, 70);
                    bottomRight = Colors(235, 235, 235);
                    break;

                case PaneFrame::Flat:
                    topLeft = m_paneFrameColor;
                    bottomRight = m_paneFrameColor;
                    break;

                case PaneFrame::NoFrame:
                    continue;
                }

                painter.fillRect(
                    left,
                    top,
                    static_cast<unsigned int>(
                        right - left + 1
                    ),
                    1,
                    topLeft
                );

                painter.fillRect(
                    left,
                    top,
                    1,
                    static_cast<unsigned int>(
                        bottom - top + 1
                    ),
                    topLeft
                );

                painter.fillRect(
                    left,
                    bottom,
                    static_cast<unsigned int>(
                        right - left + 1
                    ),
                    1,
                    bottomRight
                );

                painter.fillRect(
                    right,
                    top,
                    1,
                    static_cast<unsigned int>(
                        bottom - top + 1
                    ),
                    bottomRight
                );
            }
        }
    }

    /*
     * Paint splitter handles last.
     */

    if (m_widgets.size() < 2 ||
        m_sizes.size() != m_widgets.size())
    {
        return;
    }

    int position = 0;

    Colors handleColors(80, 80, 80);

    for (std::size_t i = 0;
         i + 1 < m_widgets.size();
         ++i)
    {
        position +=
            std::max(0, m_sizes[i]);

        if (m_orientation == Orientation::Horizontal)
        {
            painter.fillRect(
                x() + position,
                y(),
                static_cast<unsigned int>(m_handleWidth),
                static_cast<unsigned int>(
                    std::max(0, height())
                ),
                handleColors
            );
        }
        else
        {
            painter.fillRect(
                x(),
                y() + position,
                static_cast<unsigned int>(
                    std::max(0, width())
                ),
                static_cast<unsigned int>(m_handleWidth),
                handleColors
            );
        }

        position += m_handleWidth;
    }
}

} // namespace HbTK