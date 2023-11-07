#ifndef STORY_HPP
#define STORY_HPP

#include <string>

enum class StoryDialogueStatementType
{
    SPEAK = 0,
    SET_BACKGROUND = 1,
    SET_MUSIC = 2,
    NEXT_DIALOGUE = 3,
    CHOICE = 4,
    END_GAME = 5,
    DELAY = 6
};

struct StoryDialogueChoiceItem
{
    std::string text;
    std::string nextDialogueId;
    std::string ifFlag;
};

struct StoryDialogueStatement
{
    std::string text;
    std::string speaker;
    std::string nextDialogueId;
    std::string setBackgroundId;
    std::string setMusicId;
    std::vector<StoryDialogueChoiceItem> choices;
    float delayTime;

    StoryDialogueStatementType type;
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

private:
    std::vector<StoryDialogue> m_dialogues;
    std::size_t m_currentStatementIndex;
    StoryDialogue *m_currentDialogue;
};

#endif