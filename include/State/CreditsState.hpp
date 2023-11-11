#ifndef CREDITSSTATE_HPP
#define CREDITSSTATE_HPP

#include <State/State.hpp>

class CreditsState : public State
{
public:
    CreditsState(GameContext *_gameCtx);
    ~CreditsState();

    void onEnter();
    void onUpdate(float frameDelta);
    void onInput(sf::Event e);
    void onRender();
    void onLeave();

private:
    sf::Text m_creditsText;
    sf::Music m_music;

    sf::Vector2f m_creditsPos;
};

#endif
