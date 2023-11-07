#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fstream>
#include <string>

#include <SFML/System/Clock.hpp>
#include <SFML/System/Mutex.hpp>

class Logger
{
public:
    Logger();
    bool init();
    void release();

    Logger &tag(const std::string &tag);

    friend Logger &operator<<(Logger &logger, const char *text);
    friend Logger &operator<<(Logger &logger, const std::string &text);

    friend Logger &operator<<(Logger &logger, int num);
    friend Logger &operator<<(Logger &logger, unsigned int num);
    friend Logger &operator<<(Logger &logger, bool arg);
    friend Logger &operator<<(Logger &logger, float num);

private:
    sf::Clock m_clock;
    std::ofstream m_file;
    sf::Mutex m_mutex;

    bool m_firstLine;
};

Logger &GetLogger();

#endif