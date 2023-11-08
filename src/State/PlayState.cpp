#include <State/PlayState.hpp>

#include <Game/QuickShootMinigame.hpp>

PlayState::PlayState(GameContext *_gameCtx)
    : State(_gameCtx), m_dialogueUi(*_gameCtx)
{
    m_story.loadFromFile("assets/story.json");
    m_bg.setPosition(0, 0);

    sf::Image blackImg;
    blackImg.create(1920, 1080, sf::Color::Black);
    m_blackBgTexture.loadFromImage(blackImg);

    m_minigame = nullptr;

    m_bgMusic.setVolume(90);
    m_bgMusic.setLoop(true);
}

void PlayState::onEnter()
{
    m_dialogueUi.hide();
    m_story.startFromBeginning();
    m_gameContext.eval.reset();

    // DEBUG
    m_story.switchToDialogue("02_bottles_game_win");

    progressStory();
}

void PlayState::onInput(sf::Event ev)
{
    if (m_minigame && !m_minigame->isFinished())
    {
        m_minigame->input(ev);
        return;
    }

    if (ev.type == sf::Event::MouseButtonReleased)
    {
        if (ev.mouseButton.button == sf::Mouse::Left)
        {
            finishSpeakingOrProgress();
            return;
        }
    }

    if (ev.type == sf::Event::KeyReleased)
    {
        if (ev.key.code == sf::Keyboard::Space)
        {
            finishSpeakingOrProgress();
            return;
        }
    }

    if (m_dialogueUi.isShown())
    {
        auto uiResult = m_dialogueUi.input(ev);
        if (uiResult == DialogueUiInputResult::SWITCH_DIALOGUE)
        {
            m_story.switchToDialogue(m_dialogueUi.getSelectedChoice().nextDialogueId);
            progressStory();
        }
    }
}

void PlayState::onRender()
{
    if (m_minigame && !m_minigame->isFinished())
    {
        m_minigame->render();
        return;
    }

    m_gameContext.window.draw(m_bg);

    m_dialogueUi.render();
}

void PlayState::onUpdate(float frameDelta)
{
    if (m_minigame && !m_minigame->isFinished())
    {
        m_minigame->update(frameDelta);
    }
    else
    {
        checkStatementCompletion();

        m_dialogueUi.update(frameDelta);
    }
}

void PlayState::onLeave()
{
}

PlayState::~PlayState()
{
    if (m_minigame)
        delete m_minigame;
}

void PlayState::progressStory()
{
    auto stmt = m_story.getCurrentStatement();

    bool consumeNextStatement = false;

    if (stmt.condition.length())
    {
        if (!m_gameContext.eval.evalCondition(stmt.condition))
        {
            m_story.nextStatement();
            progressStory();
            return;
        }
    }

    if (stmt.type == StoryDialogueStatementType::SET_MUSIC)
    {
        if (stmt.setMusicId.length() == 0)
        {
            m_bgMusic.stop();
        }
        else
        {
            m_bgMusic.openFromFile(
                "assets/music/" + stmt.setMusicId + ".mp3");
            m_bgMusic.play();
        }

        consumeNextStatement = true;
    }
    else if (stmt.type == StoryDialogueStatementType::SET_BACKGROUND)
    {
        if (stmt.setBackgroundId.length())
        {
            m_bg.setTexture(
                m_gameContext.assetManager->getTexture(stmt.setBackgroundId));
            m_bg.setScale(
                m_gameContext.window.getSize().x / m_bg.getLocalBounds().width,
                m_gameContext.window.getSize().y / m_bg.getLocalBounds().height);
        }

        m_bg.setColor(sf::Color(255, 255, 255, 0));

        m_smoothClock.restart();

        consumeNextStatement = false;
    }
    else if (stmt.type == StoryDialogueStatementType::DELAY)
    {
        m_delayClock.restart();
        consumeNextStatement = false;
    }
    else if (stmt.type == StoryDialogueStatementType::SPEAK)
    {
        m_dialogueUi.setStatement(stmt);
        consumeNextStatement = false;
    }
    else if (stmt.type == StoryDialogueStatementType::TOGGLE_UI)
    {
        if (stmt.uiVisibilityFlag)
        {
            m_dialogueUi.show();
        }
        else
        {
            m_dialogueUi.hide();
        }
        consumeNextStatement = true;
    }
    else if (stmt.type == StoryDialogueStatementType::PLAY_SOUND)
    {
        m_storySound.setBuffer(
            m_gameContext.assetManager->getSound(stmt.soundName));
        m_storySound.play();
        consumeNextStatement = true;
    }
    else if (stmt.type == StoryDialogueStatementType::CHOICE)
    {
        m_dialogueUi.show();
        m_dialogueUi.setStatement(stmt);
        consumeNextStatement = false;
    }
    else if (stmt.type == StoryDialogueStatementType::END_GAME)
    {
        m_bgMusic.stop();
        m_gameContext.stateManager->changeState(StateType::MENU);
    }
    else if (stmt.type == StoryDialogueStatementType::NEXT_DIALOGUE)
    {
        m_story.switchToDialogue(stmt.nextDialogueId);
        progressStory();
        consumeNextStatement = false;
    }
    else if (stmt.type == StoryDialogueStatementType::MINIGAME)
    {
        if (stmt.minigame == "quick_shoot")
        {
            m_bgMusic.stop();
            m_minigame = new QuickShootMinigame(m_gameContext);
            m_minigame->start();
        }
    }
    else if (stmt.type == StoryDialogueStatementType::SET_VARIABLE)
    {
        m_gameContext.eval.setVariable(
            stmt.setVariableName);
        consumeNextStatement = true;
    }

    if (consumeNextStatement && !m_story.isDialogueFinished())
    {
        m_story.nextStatement();
        progressStory();
    }
}

void PlayState::checkStatementCompletion()
{
    auto stmt = m_story.getCurrentStatement();

    if (stmt.type == StoryDialogueStatementType::DELAY)
    {
        if (m_delayClock.getElapsedTime().asSeconds() > stmt.delayTime)
        {
            m_story.nextStatement();
            progressStory();
            return;
        }
    }
    else if (stmt.type == StoryDialogueStatementType::SET_BACKGROUND)
    {
        if (!stmt.smoothTransition || m_smoothClock.getElapsedTime().asSeconds() > 1)
        {
            m_story.nextStatement();
            progressStory();
            return;
        }

        int alpha = std::min(m_smoothClock.getElapsedTime().asSeconds() / 1.0f * 255, 255.0f);

        if (stmt.setBackgroundId.length())
        {
            m_bg.setColor(sf::Color(255, 255, 255, alpha));
        }
        else
        {
            m_bg.setColor(sf::Color(255, 255, 255, 255 - alpha));
            if (alpha >= 255)
            {
                m_bg.setTexture(m_blackBgTexture);
            }
        }
    }
    else if (stmt.type == StoryDialogueStatementType::MINIGAME)
    {
        if (m_minigame && m_minigame->isFinished())
        {
            if (m_minigame->hasUserWon())
            {
                m_story.switchToDialogue(
                    m_story.getCurrentStatement().minigameWinDialogue);
            }
            else
            {
                m_story.switchToDialogue(
                    m_story.getCurrentStatement().minigameLoseDialogue);
            }

            m_dialogueUi.show();

            progressStory();
        }
    }
}

void PlayState::finishSpeakingOrProgress()
{
    if (m_dialogueUi.isTyping())
    {
        m_dialogueUi.finishTyping();
    }
    else
    {
        if (m_story.canSkipCurrentStatement())
        {
            m_story.nextStatement();
            progressStory();
        }
    }
}