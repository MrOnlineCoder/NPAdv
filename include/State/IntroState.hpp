#ifndef INTROSTATE_HPP
#define INTROSTATE_HPP

#include <State/State.hpp>

class IntroState : public State
{
public:
    IntroState(GameContext *_gameCtx);
    ~IntroState();

    void onEnter();
    void onUpdate(float frameDelta);
    void onInput(sf::Event e);
    void onRender();
    void onLeave();

private:
    sf::Text m_introText;
    sf::Sound m_introSound;
    sf::Clock m_introClock;

    int stage;
};

#endif
