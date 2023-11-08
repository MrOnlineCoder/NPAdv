#include <Game/Story.hpp>
#include <json.hpp>
#include <fstream>
#include <Common/Logger.hpp>

#include <locale>
#include <codecvt>

Story::Story()
{
    m_dialogues.reserve(64);
    m_currentStatementIndex = 0;
}

void Story::switchToDialogue(const std::string &id)
{
    for (auto &dialogue : m_dialogues)
    {
        if (dialogue.id == id)
        {
            m_currentDialogue = &dialogue;
            m_currentStatementIndex = 0;
            return;
        }
    }
}

const StoryDialogue &Story::getCurrentDialogue()
{
    return *m_currentDialogue;
}
const StoryDialogueStatement &Story::getCurrentStatement()
{
    return m_currentDialogue->statements[m_currentStatementIndex];
}

void Story::nextStatement()
{
    if (!isDialogueFinished())
        m_currentStatementIndex++;

    GetLogger().tag("Story") << "Next statement id " << (int)m_currentStatementIndex << " in dialogue " << m_currentDialogue->id;
}

bool Story::isDialogueFinished()
{
    return m_currentStatementIndex >= m_currentDialogue->statements.size();
}

void Story::loadFromFile(const std::string &filename)
{
    std::ifstream inputFile(filename);
    nlohmann::json js = nlohmann::json::parse(inputFile);

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    for (auto node : js)
    {
        StoryDialogue dialogue;
        dialogue.id = node["dialogue_id"];
        dialogue.statements.reserve(64);

        for (auto statement : node["statements"])
        {
            StoryDialogueStatement stmt;
            stmt.speaker = L"";
            if (statement.contains("text"))
            {
                std::string utf8text = statement["text"];

                stmt.text = converter.from_bytes(utf8text);
                stmt.type = StoryDialogueStatementType::SPEAK;
            }

            if (statement.contains("speaker") && statement["speaker"].is_string())
            {
                stmt.speaker = converter.from_bytes(statement["speaker"]);
            }

            if (statement.contains("next_dialogue_id"))
            {
                stmt.nextDialogueId = statement["next_dialogue_id"];
                if (stmt.text.length() == 0)
                    stmt.type = StoryDialogueStatementType::NEXT_DIALOGUE;
            }

            if (statement.contains("set_background"))
            {
                stmt.setBackgroundId = statement["set_background"];
                stmt.type = StoryDialogueStatementType::SET_BACKGROUND;

                if (statement.contains("smooth"))
                {
                    stmt.smoothTransition = statement["smooth"];
                }
            }

            if (statement.contains("set_music"))
            {
                stmt.setMusicId = statement["set_music"];
                stmt.type = StoryDialogueStatementType::SET_MUSIC;
            }

            if (statement.contains("delay"))
            {
                stmt.delayTime = statement["delay"];
                stmt.type = StoryDialogueStatementType::DELAY;
            }

            if (statement.contains("play_sound"))
            {
                stmt.soundName = statement["play_sound"];
                stmt.type = StoryDialogueStatementType::PLAY_SOUND;
            }

            if (statement.contains("toggle_ui"))
            {
                stmt.uiVisibilityFlag = statement["toggle_ui"];
                stmt.type = StoryDialogueStatementType::TOGGLE_UI;
            }

            if (statement.contains("choices"))
            {
                for (auto choiceNode : statement["choices"])
                {
                    StoryDialogueChoiceItem choice;
                    choice.text = converter.from_bytes(choiceNode["text"]);
                    choice.nextDialogueId = choiceNode["next_dialogue_id"];
                    if (choiceNode.contains("if"))
                        choice.ifFlag = choiceNode["if"];
                    stmt.choices.push_back(choice);
                }
                stmt.type = StoryDialogueStatementType::CHOICE;
            }

            if (statement.contains("end_game"))
            {
                stmt.type = StoryDialogueStatementType::END_GAME;
            }

            if (statement.contains("minigame"))
            {
                stmt.type = StoryDialogueStatementType::MINIGAME;
                stmt.minigame = statement["minigame"];
                stmt.minigameLoseDialogue = statement["lose_dialogue_id"];
                stmt.minigameWinDialogue = statement["win_dialogue_id"];
            }

            dialogue.statements.push_back(stmt);
        }

        m_dialogues.push_back(dialogue);
        GetLogger().tag("Story") << "Loaded dialogue '" << dialogue.id << "' with " << (int)dialogue.statements.size() << " statements.";
    }

    startFromBeginning();

    GetLogger().tag("Story") << "Loaded story file '" << filename << "' with " << (int)m_dialogues.size() << " dialogues.";
}

bool Story::canSkipCurrentStatement()
{
    return getCurrentStatement().type == StoryDialogueStatementType::SPEAK;
}

void Story::startFromBeginning()
{
    m_currentStatementIndex = 0;
    m_currentDialogue = &m_dialogues[0];
}