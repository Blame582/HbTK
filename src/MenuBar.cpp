#include "MenuBar.hpp"
#include "Painter.hpp"

#include <algorithm>

namespace HbTK {

MenuBar::MenuBar()
    : m_background(Colors(192, 192, 192)),
      m_textColor(Colors(20, 20, 20)),
      m_selectedBackground(Colors(60, 90, 120)),
      m_selectedTextColor(Colors(255, 255, 255)),
      m_barHeight(28),
      m_selectedMenu(-1)
{
}

MenuBar::~MenuBar()
{
    for (Entry& entry : m_entries)
        delete entry.menu;
}

Menu* MenuBar::addMenu(const std::string& text)
{
    Menu* menu = new Menu();

    Entry entry;

    entry.text = text;
    entry.menu = menu;

    m_entries.push_back(entry);

    return menu;
}

void MenuBar::removeMenu(Menu* menu)
{
    if (menu == nullptr)
        return;

    for (std::size_t i = 0;
         i < m_entries.size();
         ++i)
    {
        if (m_entries[i].menu != menu)
            continue;

        delete m_entries[i].menu;

        m_entries.erase(
            m_entries.begin() +
            static_cast<long>(i)
        );

        if (m_selectedMenu ==
            static_cast<int>(i))
        {
            m_selectedMenu = -1;
        }
        else if (m_selectedMenu >
                 static_cast<int>(i))
        {
            --m_selectedMenu;
        }

        return;
    }
}

int MenuBar::count() const
{
    return static_cast<int>(m_entries.size());
}

Menu* MenuBar::menu(int index) const
{
    if (index < 0 || index >= count())
        return nullptr;

    return m_entries[
        static_cast<std::size_t>(index)
    ].menu;
}

const MenuBar::Entry* MenuBar::entry(int index) const
{
    if (index < 0 || index >= count())
        return nullptr;

    return &m_entries[
        static_cast<std::size_t>(index)
    ];
}

void MenuBar::setBackground(const Colors& color)
{
    m_background = color;
}

const Colors& MenuBar::background() const
{
    return m_background;
}

void MenuBar::setTextColor(const Colors& color)
{
    m_textColor = color;
}

const Colors& MenuBar::textColor() const
{
    return m_textColor;
}

void MenuBar::setSelectedBackground(const Colors& color)
{
    m_selectedBackground = color;
}

const Colors& MenuBar::selectedBackground() const
{
    return m_selectedBackground;
}

void MenuBar::setSelectedTextColor(const Colors& color)
{
    m_selectedTextColor = color;
}

const Colors& MenuBar::selectedTextColor() const
{
    return m_selectedTextColor;
}

void MenuBar::setHeight(int height)
{
    if (height < 1)
        height = 1;

    m_barHeight = height;

    resize(width(), m_barHeight);
}

int MenuBar::barHeight() const
{
    return m_barHeight;
}

int MenuBar::menuAt(int x, int y) const
{
    if (y < 0 || y >= m_barHeight)
        return -1;

    int position = 0;

    for (int i = 0; i < count(); ++i)
    {
        const Entry& entry =
            m_entries[
                static_cast<std::size_t>(i)
            ];

        const int itemWidth =
            20 +
            static_cast<int>(
                entry.text.length()
            ) * 8;

        if (x >= position &&
            x < position + itemWidth)
        {
            return i;
        }

        position += itemWidth;
    }

    return -1;
}

void MenuBar::showMenu(int index)
{
    if (index < 0 || index >= count())
        return;

    hideMenus();

    Entry& entry =
        m_entries[
            static_cast<std::size_t>(index)
        ];

    if (entry.menu == nullptr)
        return;

    int position = 0;

    for (int i = 0; i < index; ++i)
    {
        const Entry& previous =
            m_entries[
                static_cast<std::size_t>(i)
            ];

        position +=
            20 +
            static_cast<int>(
                previous.text.length()
            ) * 8;
    }

    const int menuWidth = 180;

    const int menuHeight =
        entry.menu->count() *
        entry.menu->itemHeight();

    entry.menu->resize(
        menuWidth,
        menuHeight
    );

    entry.menu->setGeometry(
        x() + position,
        y() + m_barHeight,
        menuWidth,
        menuHeight
    );

    entry.menu->show();

    m_selectedMenu = index;
}

void MenuBar::hideMenus()
{
    for (Entry& entry : m_entries)
    {
        if (entry.menu)
            entry.menu->hide();
    }
}

void MenuBar::mousePress(int x,
                         int y,
                         unsigned int button)
{
    if (button != 1)
        return;

    const int index =
        menuAt(x, y);

    if (index < 0)
    {
        hideMenus();
        m_selectedMenu = -1;
        return;
    }

    if (m_selectedMenu == index)
    {
        hideMenus();
        m_selectedMenu = -1;
        return;
    }

    showMenu(index);
}

void MenuBar::mouseMove(int x, int y)
{
    const int index =
        menuAt(x, y);

    if (index < 0)
        return;

    if (m_selectedMenu >= 0 &&
        m_selectedMenu != index)
    {
        showMenu(index);
    }
}

void MenuBar::paint(Painter& painter)
{
    if (!visible())
        return;

    painter.fillRect(
        x(),
        y(),
        static_cast<unsigned int>(
            std::max(0, width())
        ),
        static_cast<unsigned int>(
            m_barHeight
        ),
        m_background
    );

    int position = 0;

    for (int i = 0; i < count(); ++i)
    {
        const Entry& entry =
            m_entries[
                static_cast<std::size_t>(i)
            ];

        const int itemWidth =
            20 +
            static_cast<int>(
                entry.text.length()
            ) * 8;

        const bool selected =
            (i == m_selectedMenu);

        if (selected)
        {
            painter.fillRect(
                x() + position,
                y(),
                static_cast<unsigned int>(
                    itemWidth
                ),
                static_cast<unsigned int>(
                    m_barHeight
                ),
                m_selectedBackground
            );
        }

        painter.drawText(
            x() + position + 10,
            y() + (m_barHeight / 2) + 5,
            entry.text.c_str(),
            selected
                ? m_selectedTextColor
                : m_textColor
        );

        position += itemWidth;
    }
}

} // namespace HbTK