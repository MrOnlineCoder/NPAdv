#include <State/PlayState.hpp>
PlayState::PlayState(GameContext *_gameCtx)
    : State(_gameCtx)
{
    m_story.loadFromFile("assets/story.json");
    m_bg.setPosition(0, 0);

    sf::Image blackImg;
    blackImg.create(1920, 1080, sf::Color::Black);
    m_blackBgTexture.loadFromImage(blackImg);
}

void PlayState::onEnter()
{
    progressStory();
}

void PlayState::onInput(sf::Event ev)
{
}

void PlayState::onRender()
{
    m_gameContext.window.draw(m_bg);
}

void PlayState::onUpdate(float frameDelta)
{
    checkStatementCompletion();
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
                m_gameContext.window.getSize().x / m_bg.getTexture()->getSize().x,
                m_gameContext.window.getSize().y / m_bg.getTexture()->getSize().y);
        }
        else
        {
            m_bg.setTexture(m_blackBgTexture);
        }

        consumeNextStatement = true;
    }
    else if (stmt.type == StoryDialogueStatementType::DELAY)
    {
        m_delayClock.restart();
        consumeNextStatement = false;
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
        }
    }
}