#ifndef MINIGAME_HPP
#define MINIGAME_HPP

#include <GameContext.hpp>

class Minigame
{
public:
    Minigame(GameContext &context) : m_context(context)
    {
        m_finished = false;
    }

    virtual void render(){};
    virtual void update(float frameDelta){};
    virtual void input(sf::Event &ev){};

    virtual void start(){};

    bool isFinished()
    {
        return m_finished;
    };

    virtual bool hasUserWon() { return false; };

protected:
    GameContext &m_context;

    bool m_finished;
};

#endif