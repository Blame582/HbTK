#include "Menu.hpp"
#include "Painter.hpp"

#include <algorithm>

namespace HbTK {

Menu::Menu()
    : m_background(Colors(230, 230, 230)),
      m_textColor(20, 20, 20),
      m_disabledTextColor(130, 130, 130),
      m_selectedBackground(60, 90, 120),
      m_selectedTextColor(255, 255, 255),
      m_itemHeight(24),
      m_selectedItem(-1),
      m_shown(false)
{
}

Menu::~Menu()
{
}

void Menu::addItem(const std::string& text)
{
    Item item;

    item.text = text;
    item.separator = false;
    item.enabled = true;

    m_items.push_back(item);

    resize(
        width(),
        m_itemHeight * count()
    );
}

void Menu::addSeparator()
{
    Item item;

    item.text.clear();
    item.separator = true;
    item.enabled = false;

    m_items.push_back(item);

    resize(
        width(),
        m_itemHeight * count()
    );
}

void Menu::setItemEnabled(int index, bool enabled)
{
    if (index < 0 || index >= count())
        return;

    m_items[
        static_cast<std::size_t>(index)
    ].enabled = enabled;
}

bool Menu::itemEnabled(int index) const
{
    if (index < 0 || index >= count())
        return false;

    return m_items[
        static_cast<std::size_t>(index)
    ].enabled;
}

int Menu::count() const
{
    return static_cast<int>(m_items.size());
}

const Menu::Item* Menu::item(int index) const
{
    if (index < 0 || index >= count())
        return nullptr;

    return &m_items[
        static_cast<std::size_t>(index)
    ];
}

void Menu::setBackground(const Colors& color)
{
    m_background = color;
}

const Colors& Menu::background() const
{
    return m_background;
}

void Menu::setTextColor(const Colors& color)
{
    m_textColor = color;
}

const Colors& Menu::textColor() const
{
    return m_textColor;
}

void Menu::setDisabledTextColor(const Colors& color)
{
    m_disabledTextColor = color;
}

const Colors& Menu::disabledTextColor() const
{
    return m_disabledTextColor;
}

void Menu::setSelectedBackground(const Colors& color)
{
    m_selectedBackground = color;
}

const Colors& Menu::selectedBackground() const
{
    return m_selectedBackground;
}

void Menu::setSelectedTextColor(const Colors& color)
{
    m_selectedTextColor = color;
}

const Colors& Menu::selectedTextColor() const
{
    return m_selectedTextColor;
}

void Menu::setItemHeight(int height)
{
    if (height < 1)
        height = 1;

    if (m_itemHeight == height)
        return;

    m_itemHeight = height;

    resize(
        width(),
        m_itemHeight * count()
    );
}

int Menu::itemHeight() const
{
    return m_itemHeight;
}

void Menu::show()
{
    m_shown = true;
}

void Menu::hide()
{
    m_shown = false;
    m_selectedItem = -1;
}

bool Menu::isShown() const
{
    return m_shown;
}

int Menu::itemAt(int x, int y) const
{
    if (x < 0 || x >= width())
        return -1;

    if (y < 0 || y >= height())
        return -1;

    const int index =
        y / m_itemHeight;

    if (index < 0 || index >= count())
        return -1;

    return index;
}

void Menu::mousePress(int x,
                      int y,
                      unsigned int button)
{
    if (button != 1)
        return;

    const int index =
        itemAt(x, y);

    if (index < 0)
        return;

    const Item& item =
        m_items[
            static_cast<std::size_t>(index)
        ];

    if (item.separator || !item.enabled)
        return;

    m_selectedItem = index;
}

void Menu::mouseMove(int x, int y)
{
    const int index =
        itemAt(x, y);

    if (index < 0)
    {
        m_selectedItem = -1;
        return;
    }

    const Item& item =
        m_items[
            static_cast<std::size_t>(index)
        ];

    if (item.separator || !item.enabled)
    {
        m_selectedItem = -1;
        return;
    }

    m_selectedItem = index;
}

void Menu::paint(Painter& painter)
{
    if (!m_shown)
        return;

    painter.fillRect(
        x(),
        y(),
        static_cast<unsigned int>(
            std::max(0, width())
        ),
        static_cast<unsigned int>(
            std::max(0, height())
        ),
        m_background
    );

    for (int i = 0; i < count(); ++i)
    {
        const Item& item =
            m_items[
                static_cast<std::size_t>(i)
            ];

        const int itemY =
            y() + i * m_itemHeight;

        if (item.separator)
        {
            const int separatorY =
                itemY + m_itemHeight / 2;

            painter.fillRect(
                x() + 6,
                separatorY,
                static_cast<unsigned int>(
                    std::max(0, width() - 12)
                ),
                1,
                m_disabledTextColor
            );

            continue;
        }

        const bool selected =
            (i == m_selectedItem);

        if (selected)
        {
            painter.fillRect(
                x(),
                itemY,
                static_cast<unsigned int>(
                    std::max(0, width())
                ),
                static_cast<unsigned int>(
                    m_itemHeight
                ),
                m_selectedBackground
            );
        }

        const Colors& textColor =
            !item.enabled
                ? m_disabledTextColor
                : selected
                    ? m_selectedTextColor
                    : m_textColor;

        painter.drawText(
            x() + 10,
            itemY + (m_itemHeight / 2) + 5,
            item.text.c_str(),
            textColor
        );
    }
}

} // namespace HbTK