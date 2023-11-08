#include <State/IntroState.hpp>
IntroState::IntroState(GameContext *_gameCtx)
    : State(_gameCtx)
{
    m_introText.setFont(
        m_gameContext.assetManager->getFont("main"));
    m_introText.setString("Pisar Studios");
    m_introText.setFillColor(sf::Color::Black);
    m_introText.setCharacterSize(24);
    m_introText.setPosition(
        m_gameContext.window.getSize().x / 2 - m_introText.getGlobalBounds().width / 2,
        m_gameContext.window.getSize().y / 2 - m_introText.getGlobalBounds().height / 2);

    stage = 0;

    m_introSound.setBuffer(
        m_gameContext.assetManager->getSound("intro"));
}

void IntroState::onEnter()
{
    stage = 0;
}

void IntroState::onInput(sf::Event ev)
{
    if (ev.type == sf::Event::KeyReleased)
    {
        if (ev.key.code == sf::Keyboard::Escape)
        {
            m_gameContext.stateManager->changeState(StateType::MENU);
            m_introSound.stop();
        }
    }
}

void IntroState::onRender()
{
    m_gameContext.window.draw(m_introText);
}

void IntroState::onUpdate(float frameDelta)
{
    if (m_introClock.getElapsedTime().asSeconds() > 1 && stage == 0)
    {
        m_introSound.play();
        stage++;
    }

    if (m_introClock.getElapsedTime().asSeconds() > 2 && stage == 1)
    {
        m_introText.setFillColor(sf::Color::White);
        stage++;
    }

    if (m_introClock.getElapsedTime().asSeconds() > 5 && stage == 2)
    {
        m_gameContext.stateManager->changeState(StateType::MENU);
    }
}

void IntroState::onLeave()
{
}

IntroState::~IntroState()
{
}