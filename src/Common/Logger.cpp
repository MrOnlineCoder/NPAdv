#include <Common/Logger.hpp>

#include <iostream>

Logger gLogger;

Logger &GetLogger()
{
    return gLogger;
}

Logger::Logger()
{
    m_firstLine = true;
}

bool Logger::init()
{
    m_file.open("app.log");
    m_clock.restart();

    return m_file.is_open();
}

void Logger::release()
{
    m_file.flush();
    m_file.close();
}

Logger &Logger::tag(const std::string &tag)
{
    m_mutex.lock();
    if (m_firstLine)
    {
        m_file << "[" << tag << " @ " << m_clock.getElapsedTime().asSeconds() << " s.] ";
        std::cout << "[" << tag << " @ " << m_clock.getElapsedTime().asSeconds() << " s.] ";
        m_firstLine = false;
    }
    else
    {
        m_file << "\n[" << tag << " @ " << m_clock.getElapsedTime().asSeconds() << " s.] ";
        std::cout << "\n[" << tag << " @ " << m_clock.getElapsedTime().asSeconds() << " s.] ";
    }

    m_file.flush();
    std::cout.flush();

    m_mutex.unlock();

    return *this;
}

Logger &operator<<(Logger &logger, const char *text)
{
    logger.m_file << text;
    std::cout << text;
    return logger;
}

Logger &operator<<(Logger &logger, const std::string &text)
{
    logger.m_file << text;
    std::cout << text;
    return logger;
}

Logger &operator<<(Logger &logger, int num)
{
    logger.m_file << num;
    std::cout << num;
    return logger;
}

Logger &operator<<(Logger &logger, unsigned int num)
{
    logger.m_file << num;
    std::cout << num;
    return logger;
}

Logger &operator<<(Logger &logger, bool arg)
{
    logger.m_file << arg;
    std::cout << arg;
    return logger;
}

Logger &operator<<(Logger &logger, float num)
{
    logger.m_file << num;
    std::cout << num;
    return logger;
}