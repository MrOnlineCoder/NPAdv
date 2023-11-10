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

    std::random_device m_rndDevice;
    std::mt19937 m_rndGen;

    BattleshipBoard m_playerBoard;
    BattleshipBoard m_enemyBoard;

    sf::Sprite m_bg;
    sf::Music m_music;
    sf::Sound m_startSound;
};