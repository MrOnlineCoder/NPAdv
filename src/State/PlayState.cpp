#include <State/PlayState.hpp>
PlayState::PlayState(GameContext *_gameCtx)
    : State(_gameCtx), m_dialogueUi(*_gameCtx)
{
    m_story.loadFromFile("assets/story.json");
    m_bg.setPosition(0, 0);

    sf::Image blackImg;
    blackImg.create(1920, 1080, sf::Color::Black);
    m_blackBgTexture.loadFromImage(blackImg);
}

void PlayState::onEnter()
{
    m_dialogueUi.hide();
    progressStory();
}

void PlayState::onInput(sf::Event ev)
{
    if (ev.type == sf::Event::MouseButtonReleased)
    {
        if (ev.mouseButton.button == sf::Mouse::Left)
        {
            finishSpeakingOrProgress();
        }
    }

    if (ev.type == sf::Event::KeyReleased)
    {
        if (ev.key.code == sf::Keyboard::Space)
        {
            finishSpeakingOrProgress();
        }
    }
}

void PlayState::onRender()
{
    m_gameContext.window.draw(m_bg);

    m_dialogueUi.render();
}

void PlayState::onUpdate(float frameDelta)
{
    checkStatementCompletion();

    m_dialogueUi.update(frameDelta);
}

void PlayState::onLeave()
{
}

PlayState::~PlayState()
{
}

void PlayState::progressStory()
{
    auto stmt = m_story.getCurrentStatement();

    bool consumeNextStatement = false;

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
}

void PlayState::finishSpeakingOrProgress()
{
    if (m_dialogueUi.isTyping())
    {
        m_dialogueUi.finishTyping();
    }
    else
    {
        m_story.nextStatement();
        progressStory();
    }
}