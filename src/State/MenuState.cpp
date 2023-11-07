#include <State/MenuState.hpp>
MenuState::MenuState(GameContext *_gameCtx)
    : State(_gameCtx)
{
    m_titleText.setFillColor(sf::Color::Yellow);
    m_titleText.setFont(
        m_gameContext.assetManager->getFont("epic"));

    m_titleText.setCharacterSize(36);
    m_titleText.setString("Nam Pizda: The Adventure");

    m_titleText.setPosition(
        m_gameContext.window.getSize().x / 2 - m_titleText.getGlobalBounds().width / 2,
        m_gameContext.window.getSize().y / 3);

    m_music.openFromFile("assets/music/menu.mp3");

    m_playText.setCharacterSize(24);

    m_playText.setFont(
        m_gameContext.assetManager->getFont("main"));

    m_playText.setFillColor(
        sf::Color::White);

    m_playText.setString(L"Натисніть будь-яку клавішу, щоб почати гру");

    m_playText.setPosition(
        m_gameContext.window.getSize().x / 2 - m_playText.getGlobalBounds().width / 2,
        m_gameContext.window.getSize().y / 2);

    m_fadeRect.setSize(sf::Vector2f(
        m_gameContext.window.getSize().x,
        m_gameContext.window.getSize().y));

    m_fadeRect.setPosition(0, 0);
    m_fadeRect.setFillColor(sf::Color(0, 0, 0, 0));

    m_startSound.setBuffer(
        m_gameContext.assetManager->getSound("menu_start"));
}

void MenuState::onEnter()
{
    m_music.play();
    m_startingGame = false;
}

void MenuState::onInput(sf::Event ev)
{
    if (ev.type == sf::Event::KeyPressed)
    {
        m_music.stop();
        m_startingGame = true;
        m_startClock.restart();
        m_startSound.play();
    }
}

void MenuState::onRender()
{
    m_gameContext.window.draw(m_titleText);
    m_gameContext.window.draw(m_playText);

    m_gameContext.window.draw(m_fadeRect);
}

void MenuState::onUpdate(float frameDelta)
{
    if (!m_startingGame)
    {
        m_titleAnimationFactor += frameDelta;
        int newAlpha = 255 * (std::sin(m_titleAnimationFactor) + 1) / 2;
        m_playText.setFillColor(sf::Color(255, 255, 255, newAlpha));

        if (m_titleAnimationFactor > 2 * M_PI)
        {
            m_titleAnimationFactor = 0;
        }
    }
    else
    {
        const float START_FADE_TIME = 1.5f;

        const float fadeFactor = std::min(1.0f, m_startClock.getElapsedTime().asSeconds() / START_FADE_TIME);

        m_fadeRect.setFillColor(sf::Color(0, 0, 0, 255 * fadeFactor));

        if (m_startClock.getElapsedTime().asSeconds() > 4.0f)
        {
            m_gameContext.stateManager->changeState(StateType::GAME);
        }
    }
}

void MenuState::onLeave()
{
    m_music.stop();
}

MenuState::~MenuState()
{
}