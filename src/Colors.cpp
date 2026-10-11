
#include <Colors.hpp>
#include <cstdlib>
#include <cstring>

namespace HbTK {

Colors::Colors()
    : m_red(0), m_green(0), m_blue(0), m_alpha(255)
{
}

Colors::Colors(unsigned char red, unsigned char green, unsigned char blue)
    : m_red(red), m_green(green), m_blue(blue), m_alpha(255)
{
}

Colors::Colors(unsigned char red, unsigned char green, unsigned char blue,
               unsigned char alpha)
    : m_red(red), m_green(green), m_blue(blue), m_alpha(alpha)
{
}

Colors::Colors(const std::string& hex)
    : m_red(0), m_green(0), m_blue(0), m_alpha(255)
{
    const char* value = hex.c_str();

    if (*value == '#')
        ++value;

    const std::size_t length = std::strlen(value);
    if (length != 6 && length != 8)
        return;

    char* end = nullptr;
    const unsigned long rgba = std::strtoul(value, &end, 16);

    if (end == value || *end != '\0')
        return;

    if (length == 6) {
        if (rgba > 0xFFFFFF)
            return;

        m_red = static_cast<unsigned char>((rgba >> 16) & 0xFF);
        m_green = static_cast<unsigned char>((rgba >> 8) & 0xFF);
        m_blue = static_cast<unsigned char>(rgba & 0xFF);
        return;
    }

    if (rgba > 0xFFFFFFFFUL)
        return;

    m_red = static_cast<unsigned char>((rgba >> 24) & 0xFF);
    m_green = static_cast<unsigned char>((rgba >> 16) & 0xFF);
    m_blue = static_cast<unsigned char>((rgba >> 8) & 0xFF);
    m_alpha = static_cast<unsigned char>(rgba & 0xFF);
}

unsigned char Colors::red() const
{
    return m_red;
}

unsigned char Colors::green() const
{
    return m_green;
}

unsigned char Colors::blue() const
{
    return m_blue;
}

unsigned char Colors::alpha() const
{
    return m_alpha;
}

} // namespace HbTK
