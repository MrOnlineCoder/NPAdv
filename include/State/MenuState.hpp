#ifndef MENUSTATE_HPP
#define MENUSTATE_HPP

#include <State/State.hpp>

class MenuState : public State
{
public:
    MenuState(GameContext *_gameCtx);
    ~MenuState();

    void onEnter();
    void onUpdate(float frameDelta);
    void onInput(sf::Event e);
    void onRender();
    void onLeave();

private:
    sf::Text m_playText;
    sf::Text m_titleText;
    sf::Music m_music;

    float m_titleAnimationFactor;
    bool m_startingGame;
    sf::Clock m_startClock;
    sf::RectangleShape m_fadeRect;
    sf::Sound m_startSound;

    sf::Sprite m_bg;
};

#endif
