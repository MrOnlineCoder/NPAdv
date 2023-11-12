#include <Game/FightMinigame.hpp>

#include <Common/Logger.hpp>

const int WIN_THRESHOLD = 12; // change later

const int HEARTS_PADDING = 16;
const int HEARTS_MARGIN = 32;
const int MAX_HEALTH = 3;

const int ACTION_PADDING = 64;

const int MAX_COMBAT_LOG_LINES = 4;
const int COMBAT_LOG_PADDING = 32;
const int ACTION_BAR_PADDING = 64;
const int ACTION_BAR_WIDTH = 32;

FightMinigame::FightMinigame(GameContext &context) : Minigame(context), m_rndDistr(0, 100)
{
    m_bg.setTexture(m_context.assetManager->getTexture("minigame_fight"));
    m_bg.setPosition(0, 0);
    m_bg.setColor(sf::Color(255, 255, 255, 0));
    m_bg.setScale(
        m_context.window.getSize().x / m_bg.getLocalBounds().width,
        m_context.window.getSize().y / m_bg.getLocalBounds().height);

    m_rndGen.seed(m_rndDevice());

    m_music.openFromFile("assets/music/getaway.mp3");
    m_music.setVolume(80);
    m_music.setLoop(true);

    m_heart.setTexture(
        m_context.assetManager->getTexture("heart"));

    m_attackButton.setTexture(
        m_context.assetManager->getTexture("fight_attack"));

    m_defenseButton.setTexture(
        m_context.assetManager->getTexture("fight_defend"));

    m_actionText.setCharacterSize(32);
    m_actionText.setFont(
        m_context.assetManager->getFont("main"));
    m_actionText.setString(L"Оберіть дію на цей хід:");
    m_actionText.setFillColor(sf::Color::Yellow);
    m_actionText.setOutlineColor(sf::Color::Black);
    m_actionText.setOutlineThickness(0.5f);
    m_actionText.setPosition(
        m_context.window.getSize().x / 2 - m_actionText.getLocalBounds().width / 2,
        m_context.window.getSize().y / 2 - m_actionText.getLocalBounds().height / 2 - ACTION_PADDING / 2);

    m_actionTextFrame.setSize(
        sf::Vector2f(
            m_actionText.getLocalBounds().width + ACTION_PADDING,
            m_actionText.getLocalBounds().height + ACTION_PADDING));

    m_actionTextFrame.setFillColor(sf::Color(12, 53, 106, 128));
    m_actionTextFrame.setPosition(
        m_actionText.getPosition() - sf::Vector2f(ACTION_PADDING / 2, ACTION_PADDING / 2));

    m_attackButton.setPosition(
        m_context.window.getSize().x / 2 - m_attackButton.getLocalBounds().width - ACTION_PADDING / 2,
        m_context.window.getSize().y / 2 - m_attackButton.getLocalBounds().height / 2 + ACTION_PADDING);

    m_defenseButton.setPosition(
        m_context.window.getSize().x / 2 + ACTION_PADDING / 2,
        m_context.window.getSize().y / 2 - m_defenseButton.getLocalBounds().height / 2 + ACTION_PADDING);

    m_attackSound.setBuffer(
        m_context.assetManager->getSound("swing"));

    m_defendSound.setBuffer(
        m_context.assetManager->getSound("shield"));

    m_combatLogText.setFont(
        m_context.assetManager->getFont("main"));
    m_combatLogText.setCharacterSize(32);
    m_combatLogText.setFillColor(sf::Color::Black);
    m_combatLogText.setOutlineThickness(0);

    m_drawSound.setBuffer(
        m_context.assetManager->getSound("draw"));

    m_newRoundSound.setBuffer(
        m_context.assetManager->getSound("new_round"));

    m_shield.setTexture(
        m_context.assetManager->getTexture("shield"));

    m_sword.setTexture(
        m_context.assetManager->getTexture("sword"));

    m_actionBar.setFillColor(sf::Color(
        63, 112, 77));
    m_actionBarFrame.setFillColor(
        sf::Color(12, 53, 106, 128));

    m_actionBarFrame.setSize(
        sf::Vector2f(
            ACTION_BAR_WIDTH,
            m_context.window.getSize().y - ACTION_BAR_PADDING * 2));

    m_actionBarFrame.setPosition(
        m_context.window.getSize().x - ACTION_BAR_PADDING - ACTION_BAR_WIDTH,
        ACTION_BAR_PADDING);

    m_actionPointer.setTexture(
        m_context.assetManager->getTexture("fight_pointer"));
    m_actionPointer.setOrigin(
        sf::Vector2f(0, m_actionPointer.getLocalBounds().height / 2));

    m_damageSound.setBuffer(
        m_context.assetManager->getSound("damage"));
    m_blood.setTexture(
        m_context.assetManager->getTexture("blood"));
}

void FightMinigame::render()
{
    m_context.window.draw(m_bg);

    for (int i = 0; i < m_playerHp; i++)
    {
        m_heart.setPosition(
            sf::Vector2f(
                m_context.window.getSize().x / 2 - m_heart.getLocalBounds().width / 2,
                m_context.window.getSize().y - HEARTS_MARGIN - m_heart.getLocalBounds().height) +
            sf::Vector2f(
                (i - 1) * (m_heart.getLocalBounds().width + HEARTS_PADDING),
                0));
        m_context.window.draw(m_heart);
    }

    for (int i = 0; i < m_enemyHp; i++)
    {
        m_heart.setPosition(
            sf::Vector2f(
                m_context.window.getSize().x / 2 - m_heart.getLocalBounds().width / 2,
                HEARTS_MARGIN) +
            sf::Vector2f(
                (i - 1) * (m_heart.getLocalBounds().width + HEARTS_PADDING),
                0));
        m_context.window.draw(m_heart);
    }

    if (m_state == FightState::FIGHTING)
    {
        m_context.window.draw(m_combatLogText);
        if (m_turnState == TurnState::ACTION_SELECTION)
        {
            m_context.window.draw(m_actionTextFrame);
            m_context.window.draw(m_actionText);
            m_context.window.draw(m_attackButton);
            m_context.window.draw(m_defenseButton);
        }

        if (m_turnState == TurnState::ACTION_COMPARE || m_turnState == TurnState::ACTION_EXECUTION)
        {
            sf::Vector2f playerIconPos = sf::Vector2f(
                m_context.window.getSize().x / 2 - m_sword.getLocalBounds().width / 2,
                m_context.window.getSize().y - HEARTS_MARGIN * 2 - m_heart.getLocalBounds().height - m_sword.getLocalBounds().height);

            sf::Vector2f enemyIconPos = sf::Vector2f(
                m_context.window.getSize().x / 2 - m_sword.getLocalBounds().width / 2,
                HEARTS_MARGIN * 2 + m_heart.getLocalBounds().height);

            if (m_playerAction == FightAction::ATTACK)
            {
                m_sword.setPosition(playerIconPos);
                m_context.window.draw(m_sword);
            }
            else
            {
                m_shield.setPosition(playerIconPos);
                m_context.window.draw(m_shield);
            }

            if (m_compareState > 1)
            {
                if (m_enemyAction == FightAction::ATTACK)
                {
                    m_sword.setPosition(enemyIconPos);
                    m_context.window.draw(m_sword);
                }
                else
                {
                    m_shield.setPosition(enemyIconPos);
                    m_context.window.draw(m_shield);
                }
            }
        }

        if (m_turnState == TurnState::ACTION_EXECUTION)
        {
            m_context.window.draw(
                m_actionBarFrame);
            m_context.window.draw(
                m_actionBar);
            m_context.window.draw(
                m_actionPointer);
        }

        if (m_turnState == TurnState::ACTION_RESULT)
        {
            if (m_lastActionCausedDamage)
                m_context.window.draw(m_blood);
        }
    }
}

void FightMinigame::update(float delta)
{
    if (m_state == FightState::INTRO)
    {
        if (m_gameClock.getElapsedTime().asSeconds() > 0.4f)
        {
            m_gameClock.restart();
            if (m_introState == 0)
            {
                m_music.play();
            }

            if (m_introState == 1)
            {
                if (m_playerHp < MAX_HEALTH)
                {
                    m_playerHp++;
                    m_enemyHp++;
                    return;
                }

                m_bg.setColor(sf::Color(255, 255, 255, 255));
            }

            if (m_introState == 2)
            {
                m_state = FightState::FIGHTING;
                writeToCombatLog(L"Бій почався! Хід гостя.");
                m_gameClock.restart();
                return;
            }

            m_introState++;

            return;
        }
    }

    if (m_state == FightState::FIGHTING)
    {
        if (m_turnState == TurnState::ACTION_COMPARE)
        {
            if (m_compareState == 0)
            {
                if (m_gameClock.getElapsedTime().asSeconds() > 1.5f)
                {
                    m_gameClock.restart();
                    m_compareState++;
                    writeToCombatLog(L"Соболь робить вибір...");
                    return;
                }
            }

            if (m_compareState == 1)
            {
                if (m_gameClock.getElapsedTime().asSeconds() > 1.5f)
                {
                    int roll = m_rndDistr(m_rndGen);
                    if (roll > 50 || m_enemyHp <= 1 || m_playerHp <= 1)
                    {
                        m_enemyAction = FightAction::ATTACK;
                        writeToCombatLog(L"Соболь атакує!");
                        m_attackSound.play();
                    }
                    else
                    {
                        m_enemyAction = FightAction::DEFEND;
                        writeToCombatLog(L"Соболь стає в захист!");
                        m_defendSound.play();
                    }

                    m_gameClock.restart();
                    m_compareState++;
                    return;
                }
            }

            if (m_compareState == 2)
            {
                if (m_gameClock.getElapsedTime().asSeconds() > 1.5f)
                {
                    resolveTurn();

                    m_compareState = 100;
                    m_gameClock.restart();
                    return;
                }
            }

            if (m_compareState == 100)
            {
                if (m_gameClock.getElapsedTime().asSeconds() > 2.0f)
                {
                    m_newRoundSound.play();
                    writeToCombatLog(L"Новий раунд!");
                    m_turnState = TurnState::ACTION_SELECTION;
                    m_gameClock.restart();
                }
            }
        }

        if (m_turnState == TurnState::ACTION_EXECUTION)
        {
            m_actionPointerPosition += m_actionPointerDirection * m_actionPointerSpeed * delta;

            m_actionPointer.setPosition(
                m_actionBarFrame.getPosition() + sf::Vector2f(-m_actionPointer.getLocalBounds().width, m_actionPointerPosition));

            if (m_actionPointerPosition <= 0 || m_actionPointerPosition >= m_actionBarFrame.getSize().y - 1)
            {
                m_actionPointerDirection = -m_actionPointerDirection;
            }
        }

        if (m_turnState == TurnState::ACTION_RESULT)
        {
            if (m_gameClock.getElapsedTime().asSeconds() > 1.0f)
            {
                int alpha = 255 - (m_gameClock.getElapsedTime().asSeconds() - 1.0f) / 2.0f * 255;

                if (alpha <= 0)
                    alpha = 0;

                if (m_lastActionCausedDamage)
                    m_blood.setColor(sf::Color(255, 255, 255, alpha));

                if (m_gameClock.getElapsedTime().asSeconds() > 2.5f)
                {
                    if (m_enemyHp <= 0 || m_playerHp <= 0)
                    {
                        m_finished = true;
                        m_music.stop();
                    }

                    m_turnState = TurnState::ACTION_COMPARE;
                }
            }
        }
    }
}

void FightMinigame::start()
{
    m_finished = false;
    m_score = 0;
    m_state = FightState::INTRO;
    m_introState = 0;
    m_enemyHp = 0;
    m_playerHp = 0;
    m_turnState = TurnState::ACTION_SELECTION;
}

void FightMinigame::input(sf::Event &ev)
{
    if (ev.type == sf::Event::MouseButtonReleased)
    {
        if (ev.mouseButton.button == sf::Mouse::Left)
        {
            if (m_state == FightState::FIGHTING)
            {
                if (m_turnState == TurnState::ACTION_SELECTION)
                {
                    sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(m_context.window));

                    if (m_attackButton.getGlobalBounds().contains(mousePos))
                    {
                        selectPlayerAction(FightAction::ATTACK);
                        return;
                    }

                    if (m_defenseButton.getGlobalBounds().contains(mousePos))
                    {
                        selectPlayerAction(FightAction::DEFEND);
                        return;
                    }
                }
            }
        }
    }

    if (ev.type == sf::Event::KeyPressed)
    {
        if (m_turnState == TurnState::ACTION_EXECUTION && ev.key.code == sf::Keyboard::Space)
        {
            performAction();
        }
    }
}

bool FightMinigame::hasUserWon()
{
    return m_enemyHp == 0 && m_playerHp > 0;
}

void FightMinigame::writeToCombatLog(std::wstring str)
{
    m_combatLog.push_back(str);

    if (m_combatLog.size() > MAX_COMBAT_LOG_LINES)
    {
        m_combatLog.erase(m_combatLog.begin());
    }

    std::wstring combatLogText = L"";

    for (int i = 0; i < m_combatLog.size(); i++)
    {
        combatLogText += m_combatLog[i] + L"\n";
    }

    m_combatLogText.setString(combatLogText);
    m_combatLogText.setPosition(
        COMBAT_LOG_PADDING,
        m_context.window.getSize().y / 2 - m_combatLogText.getLocalBounds().height / 2);
}

void FightMinigame::selectPlayerAction(FightAction action)
{
    m_turnState = TurnState::ACTION_COMPARE;
    m_playerAction = action;
    if (action == FightAction::DEFEND)
    {
        writeToCombatLog(L"Гість вирішує стати в захист!");
        m_defendSound.play();
    }
    else
    {
        writeToCombatLog(L"Гість вирішує атакувати!");
        m_attackSound.play();
    }

    m_gameClock.restart();

    m_compareState = 0;
    return;
}

void FightMinigame::resolveTurn()
{
    if (m_enemyAction == m_playerAction && m_playerAction == FightAction::DEFEND)
    {
        writeToCombatLog(L"Обидва учасники стали в захист. Нічия!");
        m_drawSound.play();
        return;
    }

    if (m_enemyAction == m_playerAction && m_playerAction == FightAction::ATTACK)
    {
        int roll = m_rndDistr(m_rndGen);

        m_turnState = TurnState::ACTION_EXECUTION;

        if (roll >= 50)
        {
            writeToCombatLog(L"Гість контратакує!");
            m_targetAction = FightAction::ATTACK;
        }
        else
        {
            writeToCombatLog(L"Cоболь контратакує!");
            m_targetAction = FightAction::DEFEND;
        }

        prepareActionBar();
        return;
    }

    if (m_enemyAction == FightAction::ATTACK && m_playerAction == FightAction::DEFEND)
    {
        writeToCombatLog(L"Соболь атакує гостя в захисті...");
        int roll = m_rndDistr(m_rndGen);

        if (roll < 33)
        {
            writeToCombatLog(L"Промах!");
            return;
        }

        m_targetAction = FightAction::DEFEND;
        m_turnState = TurnState::ACTION_EXECUTION;
        prepareActionBar();
        return;
    }

    if (m_enemyAction == FightAction::DEFEND && m_playerAction == FightAction::ATTACK)
    {
        writeToCombatLog(L"Гість атакує соболя в захисті...");
        int roll = m_rndDistr(m_rndGen);

        if (roll < 33)
        {
            writeToCombatLog(L"Промах!");
            return;
        }

        m_targetAction = FightAction::ATTACK;
        m_turnState = TurnState::ACTION_EXECUTION;
        prepareActionBar();
    }
}

void FightMinigame::prepareActionBar()
{
    const int MAX_BAR_VALUE = m_context.window.getSize().y - ACTION_BAR_PADDING * 2;
    const int BASE_BAR_SPEED = 450;
    const int BASE_BAR_HEIGHT = MAX_BAR_VALUE / 4;

    std::uniform_int_distribution<> distr(0, MAX_BAR_VALUE);

    m_actionPointerSpeed = (MAX_HEALTH + 1 - m_enemyHp) * BASE_BAR_SPEED;
    m_actionPointerSuccessHeight = BASE_BAR_HEIGHT / (MAX_HEALTH + 1 - m_enemyHp);

    std::uniform_int_distribution<> pdistr(0, MAX_BAR_VALUE - m_actionPointerSuccessHeight);
    m_actionPointerSuccessPosition = pdistr(m_rndGen);

    GetLogger().tag("FightMinigame") << "Pointer speed: " << m_actionPointerSpeed << ", success height: " << m_actionPointerSuccessHeight << ", success position: " << m_actionPointerSuccessPosition << ".";

    m_actionPointerPosition = 0;

    m_actionBar.setSize(
        sf::Vector2f(
            ACTION_BAR_WIDTH,
            m_actionPointerSuccessHeight));

    m_actionBar.setPosition(
        m_actionBarFrame.getPosition() + sf::Vector2f(0, m_actionPointerSuccessPosition));

    m_actionPointerDirection = 1;

    GetLogger().tag("FightMinigame") << "Lets go!";
}

void FightMinigame::performAction()
{
    bool isSuccess = m_actionPointerPosition >= m_actionPointerSuccessPosition && m_actionPointerPosition <= m_actionPointerSuccessPosition + m_actionPointerSuccessHeight;

    m_lastActionCausedDamage = false;

    if (m_targetAction == FightAction::ATTACK)
    {
        if (isSuccess)
        {
            writeToCombatLog(L"Гість наносить удар!");
            m_enemyHp--;
            m_blood.setColor(sf::Color(255, 255, 255, 255));
            m_damageSound.play();
            m_lastActionCausedDamage = true;
        }
        else
        {
            writeToCombatLog(L"Гість промахується!");
        }
    }

    if (m_targetAction == FightAction::DEFEND)
    {
        if (isSuccess)
        {
            writeToCombatLog(L"Гість відбиває удар!");
        }
        else
        {
            writeToCombatLog(L"Соболь наносить удар!");
            m_playerHp--;
            m_blood.setColor(sf::Color(255, 255, 255, 255));
            m_damageSound.play();
            m_lastActionCausedDamage = true;
        }
    }

    m_turnState = TurnState::ACTION_RESULT;

    m_gameClock.restart();
}
