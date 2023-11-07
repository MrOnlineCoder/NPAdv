#ifndef MATH_UTILS_HPP
#define MATH_UTILS_HPP

#include <SFML/Graphics.hpp>

namespace MathUtils
{
    sf::Vector2f lerp(sf::Vector2f a, sf::Vector2f b, float f);
    float distance(sf::Vector2f a, sf::Vector2f b);
    sf::Vector2f normalize(sf::Vector2f a);
    float angleBetweenVectors(sf::Vector2f a, sf::Vector2f b);
    float radiansToDegrees(float radians);
}

#endif