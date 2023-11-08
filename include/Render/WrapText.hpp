#ifndef WRAP_TEXT_HPP
#define WRAP_TEXT_HPP

#include <string>

#include <SFML/Graphics.hpp>
namespace RenderUtils
{
    std::wstring wrapText(std::wstring string, unsigned width, const sf::Font &font, unsigned charicterSize, bool bold = false)
    {
        unsigned currentOffset = 0;
        bool firstWord = true;
        std::size_t wordBegining = 0;

        for (std::size_t pos(0); pos < string.size(); ++pos)
        {
            auto currentChar = string[pos];
            if (currentChar == '\n')
            {
                currentOffset = 0;
                firstWord = true;
                continue;
            }
            else if (currentChar == ' ')
            {
                wordBegining = pos;
                firstWord = false;
            }

            auto glyph = font.getGlyph(currentChar, charicterSize, bold);
            currentOffset += glyph.advance;

            if (!firstWord && currentOffset > width)
            {
                pos = wordBegining;
                string[pos] = L'\n';
                firstWord = true;
                currentOffset = 0;
            }
        }

        return string;
    }
}

#endif
