#include <Colors.hpp>

#include <cstdlib>
#include <cstring>

namespace HbTK {

Colors::Colors()
    : m_red(0), m_green(0), m_blue(0)
{
}

Colors::Colors(unsigned char red, unsigned char green, unsigned char blue)
    : m_red(red), m_green(green), m_blue(blue)
{
}

Colors::Colors(const std::string& hex)
    : m_red(0), m_green(0), m_blue(0)
{
    const char* value = hex.c_str();

    if (*value == '#')
        ++value;

    if (std::strlen(value) != 6)
        return;

    char* end = nullptr;
    unsigned long rgb = std::strtoul(value, &end, 16);

    if (end == value || *end != '\0' || rgb > 0xFFFFFF)
        return;

    m_red = static_cast<unsigned char>((rgb >> 16) & 0xFF);
    m_green = static_cast<unsigned char>((rgb >> 8) & 0xFF);
    m_blue = static_cast<unsigned char>(rgb & 0xFF);
}

unsigned char Colors::red() const { return m_red; }
unsigned char Colors::green() const { return m_green; }
unsigned char Colors::blue() const { return m_blue; }

} // namespace HbTK