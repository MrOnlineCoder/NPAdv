#include <Game/QuickShootMinigame.hpp>

#include <Common/Logger.hpp>

const int TARGET_MIN_PADDING = 20;
const int MAX_TARGETS = 20;
const int WIN_THRESHOLD = 14; // change later
const int INITIAL_SECONDS_LEFT = 15;

QuickShootMinigame::QuickShootMinigame(GameContext &context) : Minigame(context), m_rndDistr(1, MAX_TARGETS)
{
    m_bg.setTexture(m_context.assetManager->getTexture("minigame_quickshoot"));
    m_bg.setPosition(0, 0);
    m_bg.setScale(
        m_context.window.getSize().x / m_bg.getLocalBounds().width,
        m_context.window.getSize().y / m_bg.getLocalBounds().height);

    m_timerText.setFont(
        m_context.assetManager->getFont("main"));
    m_timerText.setCharacterSize(32);
    m_timerText.setString("00:15");
    m_timerText.setFillColor(sf::Color::Black);
    m_timerText.setPosition(
        sf::Vector2f(
            m_context.window.getSize().x / 2 - m_timerText.getLocalBounds().width / 2,
            32));

    m_scoreText.setCharacterSize(48);
    m_scoreText.setFont(
        m_context.assetManager->getFont("main"));
    m_scoreText.setFillColor(sf::Color::Black);
    m_scoreText.setString("0");
    m_scoreText.setStyle(sf::Text::Bold);
    m_scoreText.setPosition(
        sf::Vector2f(
            m_context.window.getSize().x / 2 - m_scoreText.getLocalBounds().width / 2,
            m_context.window.getSize().y / 2 - m_scoreText.getLocalBounds().height / 2));

    m_state = QuickShootState::INTRO;

    m_introSound.setBuffer(
        m_context.assetManager->getSound("revolver_cock"));
    m_startSound.setBuffer(
        m_context.assetManager->getSound("sport_whistle"));

    m_music.openFromFile("assets/music/amok.mp3");
    m_music.stop();

    m_shotSound.setBuffer(
        m_context.assetManager->getSound("revolver_shot"));

    m_rndGen.seed(m_rndDevice());

    m_crosshair.setTexture(
        m_context.assetManager->getTexture("crosshair"));

    m_crosshair.setOrigin(
        sf::Vector2f(
            m_crosshair.getLocalBounds().width / 2,
            m_crosshair.getLocalBounds().height / 2)

    );
}

void QuickShootMinigame::render()
{
    m_context.window.draw(m_bg);
    m_context.window.draw(m_timerText);
    m_context.window.draw(m_scoreText);

    if (m_state == QuickShootState::SHOOTING)
    {
        m_context.window.draw(m_target);
    }

    m_context.window.draw(m_crosshair);
}

void QuickShootMinigame::update(float delta)
{
    m_crosshair.setPosition(
        sf::Vector2f(sf::Mouse::getPosition(m_context.window)));

    if (m_state == QuickShootState::INTRO)
    {
        if (m_gameClock.getElapsedTime().asSeconds() > 2)
        {
            m_startSound.play();
            m_music.play();
            m_state = QuickShootState::SHOOTING;
            nextTarget();
        }
        return;
    }

    if (m_state == QuickShootState::END)
    {
        if (m_gameClock.getElapsedTime().asSeconds() > 2.5f)
        {
            m_finished = true;
            m_context.window.setMouseCursorVisible(true);
        }
        return;
    }

    if (m_gameClock.getElapsedTime().asSeconds() >= 1)
    {
        m_gameClock.restart();
        m_secondsLeft--;
        if (m_secondsLeft < 10)
        {
            m_timerText.setString("00:0" + std::to_string(m_secondsLeft));
        }
        else
        {
            m_timerText.setString("00:" + std::to_string(m_secondsLeft));
        }
        if (m_secondsLeft <= 0)
        {
            m_state = QuickShootState::END;
            m_startSound.play();
            m_music.stop();
            m_gameClock.restart();
        }
    }
}

void QuickShootMinigame::start()
{
    m_finished = false;
    m_secondsLeft = INITIAL_SECONDS_LEFT;
    m_score = 0;

    m_state = QuickShootState::INTRO;
    m_introSound.play();
    m_context.window.setMouseCursorVisible(false);
}

void QuickShootMinigame::nextTarget()
{
    int random_target = m_rndDistr(m_rndGen);

    GetLogger().tag("QuickShoot") << "New target number is " << random_target;

    std::string texture_name = "target_" + std::to_string(random_target);
    m_target.setTexture(m_context.assetManager->getTexture(texture_name));

    std::uniform_int_distribution<> xposDistr(
        TARGET_MIN_PADDING,
        m_context.window.getSize().x - m_target.getLocalBounds().width - TARGET_MIN_PADDING);

    std::uniform_int_distribution<> yposDistr(
        TARGET_MIN_PADDING,
        m_context.window.getSize().y - m_target.getLocalBounds().height - TARGET_MIN_PADDING);

    m_target.setPosition(
        xposDistr(m_rndGen),
        yposDistr(m_rndGen));
}

void QuickShootMinigame::input(sf::Event &ev)
{
    if (ev.type == sf::Event::MouseButtonPressed && ev.mouseButton.button == sf::Mouse::Left)
    {

        if (m_target.getGlobalBounds().contains(ev.mouseButton.x, ev.mouseButton.y))
        {
            m_score++;
            m_scoreText.setString(
                std::to_string(m_score));
            m_shotSound.play();
            nextTarget();
        }
    }
}

bool QuickShootMinigame::hasUserWon()
{
    return m_score >= WIN_THRESHOLD;
}