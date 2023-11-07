#ifndef STATE_HPP
#define STATE_HPP

#include <SFML/Window/Event.hpp>

#include <GameContext.hpp>

class State
{
public:
    State(GameContext *_gameCtx) : m_gameContext(*_gameCtx) {}
    virtual ~State(){};

    virtual void onEnter() = 0;
    virtual void onUpdate(float frameDelta) = 0;
    virtual void onInput(sf::Event e) = 0;
    virtual void onRender() = 0;
    virtual void onLeave() = 0;

protected:
    GameContext &m_gameContext;
};

#endif