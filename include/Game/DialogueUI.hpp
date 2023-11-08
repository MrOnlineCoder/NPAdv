#ifndef DIALOGUE_UI_HPP
#define DIALOGUE_UI_HPP

#include <GameContext.hpp>
#include <Game/Story.hpp>

#include <vector>

enum class DialogueUiInputResult
{
    NONE,
    SWITCH_DIALOGUE
};

class DialogueUI
{
public:
    DialogueUI(GameContext &_context);

    void setStatement(const StoryDialogueStatement &stmt);
    void update(float frameDelta);
    void render();
    DialogueUiInputResult input(sf::Event &ev);

    void finishTyping();
    bool isTyping();

    void hide();
    void show();
    bool isShown();

    StoryDialogueChoiceItem getSelectedChoice();

private:
    void fitText();

    bool isShowingChoices();

    GameContext &m_context;

    sf::RectangleShape m_textFrame;
    sf::Text m_text;
    sf::Text m_speakerName;
    sf::RectangleShape m_speakerFrame;

    sf::Clock m_typingClock;

    std::vector<sf::Text> m_choiceTexts;

    bool m_isTyping;
    bool m_isShown;

    std::size_t m_lastTypedPosition;

    StoryDialogueStatement m_currentStatement;
    StoryDialogueChoiceItem m_selectedChoice;
};

#endif