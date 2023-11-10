#include <Game/Minigame.hpp>
#include <random>

const int BATTLESHIP_BOARD_SIZE = 10;

enum class BattleshipCellType
{
    EMPTY,
    SHIP,
    HIT,
    MISS
};

enum class BattleshipTurnState
{
    THINKING,
    SHOOTING,
    OUTCOME
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
    void renderBoard(BattleshipBoard &board, sf::Vector2f startPos, bool hideShips);

    bool shoot(BattleshipBoard &board, int x, int y);

    void setTurnStatusText(std::wstring str);

    std::random_device m_rndDevice;
    std::mt19937 m_rndGen;

    BattleshipBoard m_playerBoard;
    BattleshipBoard m_enemyBoard;

    BattleshipTurnState m_turnState;

    sf::Sprite m_bg;
    sf::Music m_music;
    sf::Sound m_startSound;

    sf::RectangleShape m_gridHorizontalLine;
    sf::RectangleShape m_gridVerticalLine;
    sf::RectangleShape m_boardBackground;
    sf::RectangleShape m_hoverTile;

    sf::Clock m_turnClock;
    bool m_isPlayerTurn;

    sf::Text m_turnText;
};