#ifndef DIALOGUE_UI_HPP
#define DIALOGUE_UI_HPP

#include <GameContext.hpp>
#include <Game/Story.hpp>

class DialogueUI
{
public:
    DialogueUI(GameContext &_context);

    void setStatement(const StoryDialogueStatement &stmt);
    void update(float frameDelta);
    void render();

    void finishTyping();
    bool isTyping();

    void hide();
    void show();

private:
    void fitText();

    GameContext &m_context;

    sf::RectangleShape m_textFrame;
    sf::Text m_text;
    sf::Text m_speakerName;
    sf::RectangleShape m_speakerFrame;

    sf::Clock m_typingClock;

    bool m_isTyping;
    bool m_isShown;

    std::size_t m_lastTypedPosition;

    StoryDialogueStatement m_currentStatement;
};

#endif