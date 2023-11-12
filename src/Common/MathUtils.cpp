#include <Common/MathUtils.hpp>

#define _USE_MATH_DEFINES
#include <cmath>

namespace MathUtils
{
    sf::Vector2f lerp(sf::Vector2f a, sf::Vector2f b, float f)
    {
        return sf::Vector2f(
            a.x * (1.0 - f) + (b.x * f),
            a.y * (1.0 - f) + (b.y * f));
    }

    float distance(sf::Vector2f a, sf::Vector2f b)
    {
        return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
    }

    sf::Vector2f normalize(sf::Vector2f a)
    {
        float length = sqrt(pow(a.x, 2) + pow(a.y, 2));
        return sf::Vector2f(a.x / length, a.y / length);
    }

    float angleBetweenVectors(sf::Vector2f a, sf::Vector2f b)
    {
        float dot = a.x * b.x + a.y * b.y;
        float det = a.x * b.y - a.y * b.x;
        return radiansToDegrees(atan2(det, dot));
    }

    float radiansToDegrees(float radians)
    {
        return radians * 180.0 / 3.14159265358979323846;
    }
}