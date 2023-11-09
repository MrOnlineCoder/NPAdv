#include <Game/Minigame.hpp>
#include <random>
#include <vector>

enum class FightState
{
    INTRO,
    FIGHTING,
    END
};

enum class TurnState
{
    ACTION_SELECTION,
    ACTION_COMPARE,
    ACTION_EXECUTION,
    ACTION_RESULT
};

enum class FightAction
{
    ATTACK,
    DEFEND
};

class FightMinigame : public Minigame
{
public:
    FightMinigame(GameContext &context);

    void render();
    void update(float delta);
    void start();
    void input(sf::Event &ev);

    bool hasUserWon();

private:
    void writeToCombatLog(std::wstring str);
    void selectPlayerAction(FightAction action);
    void resolveTurn();
    void prepareActionBar();
    void performAction();

    sf::Sprite m_bg;

    sf::Sound m_startSound;
    sf::Clock m_gameClock;
    int m_score;
    FightState m_state;
    sf::Music m_music;
    int m_introState;
    int m_playerHp;
    int m_enemyHp;
    TurnState m_turnState;
    FightAction m_playerAction;
    FightAction m_enemyAction;
    sf::Sound m_attackSound;
    sf::Sound m_defendSound;
    sf::Sound m_drawSound;
    sf::Sound m_newRoundSound;
    int m_compareState;

    std::vector<std::wstring> m_combatLog;
    sf::Text m_combatLogText;

    sf::Sprite m_target;
    sf::Sprite m_heart;
    sf::Sprite m_attackButton;
    sf::Sprite m_defenseButton;
    sf::Text m_actionText;
    sf::RectangleShape m_actionTextFrame;
    sf::Sound m_damageSound;
    sf::Sprite m_blood;

    bool m_lastActionCausedDamage;

    sf::Sprite m_shield;
    sf::Sprite m_sword;

    float m_actionPointerPosition;
    float m_actionPointerSuccessPosition;
    float m_actionPointerSpeed;
    float m_actionPointerSuccessHeight;
    int m_actionPointerDirection;

    sf::Sprite m_actionPointer;
    sf::RectangleShape m_actionBar;
    sf::RectangleShape m_actionBarFrame;

    FightAction m_targetAction;

    std::random_device m_rndDevice;
    std::mt19937 m_rndGen;
    std::uniform_int_distribution<> m_rndDistr;
};