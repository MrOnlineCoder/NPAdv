#ifndef PLAYSTATE_HPP
#define PLAYSTATE_HPP

#include <State/State.hpp>

#include <Game/Story.hpp>

class PlayState : public State
{
public:
    PlayState(GameContext *_gameCtx);
    ~PlayState();

    void onEnter();
    void onUpdate(float frameDelta);
    void onInput(sf::Event e);
    void onRender();
    void onLeave();

    void progressStory();

private:
    void checkStatementCompletion();

    Story m_story;

    sf::Music m_bgMusic;
    sf::Sprite m_bg;
    sf::Texture m_blackBgTexture;

    sf::Clock m_delayClock;
};

#endif
