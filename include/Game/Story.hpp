#ifndef STORY_HPP
#define STORY_HPP

#include <string>

#include <vector>

enum class StoryDialogueStatementType
{
    SPEAK = 0,
    SET_BACKGROUND = 1,
    SET_MUSIC = 2,
    NEXT_DIALOGUE = 3,
    CHOICE = 4,
    END_GAME = 5,
    DELAY = 6,
    TOGGLE_UI = 7,
    PLAY_SOUND = 8,
    MINIGAME = 9,
    SET_VARIABLE = 10
};

struct StoryDialogueChoiceItem
{
    std::wstring text;
    std::string nextDialogueId;
    std::string ifFlag;
};

struct StoryDialogueStatement
{
    std::wstring text;
    std::wstring speaker;
    std::string nextDialogueId;
    std::string setBackgroundId;
    std::string setMusicId;
    std::string soundName;
    std::vector<StoryDialogueChoiceItem> choices;
    float delayTime;
    bool uiVisibilityFlag;
    bool smoothTransition;
    std::string condition;

    std::string minigame;
    std::string minigameWinDialogue;
    std::string minigameLoseDialogue;

    StoryDialogueStatementType type;

    std::string setVariableName;
};

struct StoryDialogue
{
    std::string id;
    std::vector<StoryDialogueStatement> statements;
};

class Story
{
public:
    Story();

    void switchToDialogue(const std::string &id);
    const StoryDialogue &getCurrentDialogue();
    const StoryDialogueStatement &getCurrentStatement();
    void nextStatement();

    void loadFromFile(const std::string &filename);

    bool isDialogueFinished();

    bool canSkipCurrentStatement();

    void startFromBeginning();

private:
    std::vector<StoryDialogue> m_dialogues;
    std::size_t m_currentStatementIndex;
    StoryDialogue *m_currentDialogue;
};

#endif