#ifndef PLAYSTATE_HPP
#define PLAYSTATE_HPP

#include <State/State.hpp>

#include <Game/Story.hpp>
#include <Game/DialogueUI.hpp>

#include <Game/Minigame.hpp>

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
    void finishSpeakingOrProgress();

    Story m_story;
    DialogueUI m_dialogueUi;

    sf::Music m_bgMusic;
    sf::Sprite m_bg;
    sf::Texture m_blackBgTexture;

    sf::Clock m_delayClock;
    sf::Clock m_smoothClock;
    sf::Sound m_storySound;

    Minigame *m_minigame;
};

#endif
