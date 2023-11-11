#include <Game/Minigame.hpp>
#include <random>
#include <Render/AnimatedSprite.hpp>

const int BATTLESHIP_BOARD_SIZE = 10;

enum class BattleshipCellType
{
    EMPTY,
    SHIP,
    HIT,
    MISS,
    SUNK
};

enum class BattleshipTurnState
{
    THINKING,
    SHOOTING,
    OUTCOME,
    FINISHED
};

typedef BattleshipCellType BattleshipBoard[BATTLESHIP_BOARD_SIZE][BATTLESHIP_BOARD_SIZE];

class BattleshipsMinigame : public Minigame
{
public:
    BattleshipsMinigame(GameContext &context);

    void render();
    void update(float delta);
    void start();
    void input(sf::Event &ev);

    bool hasUserWon();

private:
    void prepareBoard(BattleshipBoard &board);
    void addShip(int size, BattleshipBoard &board);
    bool tryAddingShipAtPosition(BattleshipBoard &board, int size, int x, int y, int dir);
    void renderBoard(BattleshipBoard &board, sf::Vector2f startPos, bool hideShips);

    bool shoot(BattleshipBoard &board, int x, int y);
    void checkDamage();
    void executeEnemyTurn();

    void setTurnStatusText(std::wstring str);

    sf::Vector2i tryGetCellAtMouse(sf::Vector2f boardPos);

    std::random_device m_rndDevice;
    std::mt19937 m_rndGen;

    BattleshipBoard m_playerBoard;
    BattleshipBoard m_enemyBoard;

    BattleshipTurnState m_turnState;

    sf::Sprite m_bg;
    sf::Music m_music;
    sf::Sound m_startSound;

    sf::Sound m_missSound;
    sf::Sound m_hitSound;
    sf::Sound m_shootSound;
    sf::RectangleShape m_gridHorizontalLine;
    sf::RectangleShape m_gridVerticalLine;
    sf::RectangleShape m_boardBackground;
    sf::RectangleShape m_hoverTile;
    sf::Sprite m_missIcon;
    sf::Sprite m_shipIcon;
    sf::Sprite m_sunkShipIcon;
    AnimatedSprite m_fireIcon;
    Animation m_fireAnimation;

    sf::Vector2i m_targetShootPos;
    sf::Vector2i m_lastEnemyHitPosition;
    bool m_lastEnemyHitWasSuccessful;
    bool m_debugEnemyShips;

    bool m_hasPlayerWon;

    sf::Vector2f m_enemyBoardStartPosition;

    sf::Clock m_turnClock;
    bool m_isPlayerTurn;

    sf::Text m_turnText;
};