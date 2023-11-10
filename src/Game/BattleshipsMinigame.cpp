#include <Game/BattleshipsMinigame.hpp>
#include <Common/Logger.hpp>

BattleshipsMinigame::BattleshipsMinigame(GameContext &context) : Minigame(context)
{
    m_bg.setTexture(m_context.assetManager->getTexture("minigame_ships"));
    m_bg.setPosition(0, 0);
    m_bg.setScale(
        m_context.window.getSize().x / m_bg.getLocalBounds().width,
        m_context.window.getSize().y / m_bg.getLocalBounds().height);

    m_music.openFromFile("assets/music/five_armies.mp3");
    m_music.play();
    m_music.setLoop(true);

    m_rndGen.seed(m_rndDevice());
}

void BattleshipsMinigame::render()
{
    m_context.window.draw(m_bg);
}

void BattleshipsMinigame::update(float delta)
{
}

void BattleshipsMinigame::start()
{
    prepareBoard(m_playerBoard);
    prepareBoard(m_enemyBoard);

    printf("\n");
    for (int i = 0; i < BATTLESHIP_BOARD_SIZE; i++)
    {
        for (int j = 0; j < BATTLESHIP_BOARD_SIZE; j++)
        {
            printf("%d ", m_playerBoard[i][j]);
        }
        printf("\n");
    }
}

void BattleshipsMinigame::input(sf::Event &ev)
{
}

bool BattleshipsMinigame::hasUserWon()
{
    return false;
}

void BattleshipsMinigame::prepareBoard(BattleshipBoard &board)
{

    for (int i = 0; i < BATTLESHIP_BOARD_SIZE; i++)
    {
        for (int j = 0; j < BATTLESHIP_BOARD_SIZE; j++)
        {
            board[i][j] = BattleshipCellType::EMPTY;
        }
    }

    // 4x1
    addShip(1, board);
    addShip(1, board);
    addShip(1, board);
    addShip(1, board);

    // 3x2
    addShip(2, board);
    addShip(2, board);
    addShip(2, board);

    // 2x3
    addShip(3, board);
    addShip(3, board);

    // 1x4
    addShip(4, board);
}

void BattleshipsMinigame::addShip(int size, BattleshipBoard &board)
{
    std::uniform_int_distribution<> boardDistr(0, BATTLESHIP_BOARD_SIZE - size);
    std::uniform_int_distribution<> dirDistr(0, 1);
    int n = 0;

    int xPos = boardDistr(m_rndGen);
    int yPos = boardDistr(m_rndGen);

    while (n < BATTLESHIP_BOARD_SIZE * BATTLESHIP_BOARD_SIZE)
    {
        int dir = dirDistr(m_rndGen);

        bool occupied = false;

        for (int i = 0; i < size; i++)
        {
            int cxPos = dir ? xPos + i : xPos;
            int cyPos = dir ? yPos : yPos + i;

            if (cxPos < 0 || cxPos >= BATTLESHIP_BOARD_SIZE ||
                cyPos < 0 || cyPos >= BATTLESHIP_BOARD_SIZE)
            {
                continue;
            }

            if (board[cxPos][cyPos] != BattleshipCellType::EMPTY)
            {
                occupied = true;
                break;
            }

            if (cxPos + 1 < BATTLESHIP_BOARD_SIZE && board[cxPos + 1][cyPos] != BattleshipCellType::EMPTY)
            {
                occupied = true;
                break;
            }

            if (cxPos - 1 >= 0 && board[cxPos - 1][cyPos] != BattleshipCellType::EMPTY)
            {
                occupied = true;
                break;
            }

            if (cyPos - 1 >= 0 && board[cxPos][cyPos - 1] != BattleshipCellType::EMPTY)
            {
                occupied = true;
                break;
            }

            if (cyPos + 1 < BATTLESHIP_BOARD_SIZE && board[cxPos][cyPos + 1] != BattleshipCellType::EMPTY)
            {
                occupied = true;
                break;
            }

            if (cyPos - 1 >= 0 && cxPos - 1 >= 0 && board[cxPos - 1][cyPos - 1] != BattleshipCellType::EMPTY)
            {
                occupied = true;
                break;
            }

            if (cyPos + 1 < BATTLESHIP_BOARD_SIZE && cxPos - 1 >= 0 && board[cxPos - 1][cyPos + 1] != BattleshipCellType::EMPTY)
            {
                occupied = true;
                break;
            }

            if (cyPos - 1 >= 0 && cxPos + 1 < BATTLESHIP_BOARD_SIZE && board[cxPos + 1][cyPos - 1] != BattleshipCellType::EMPTY)
            {
                occupied = true;
                break;
            }

            if (cyPos + 1 < BATTLESHIP_BOARD_SIZE && cxPos + 1 < BATTLESHIP_BOARD_SIZE && board[cxPos + 1][cyPos + 1] != BattleshipCellType::EMPTY)
            {
                occupied = true;
                break;
            }
        }

        if (occupied)
        {
            xPos++;
            yPos++;

            if (xPos > BATTLESHIP_BOARD_SIZE - size)
            {
                xPos = 0;
                yPos++;
            }

            if (yPos > BATTLESHIP_BOARD_SIZE - size)
            {
                yPos = 0;
                xPos = 0;
            }
            n++;
            continue;
        }

        for (int i = 0; i < size; i++)
        {
            if (dir == 1)
            {
                board[xPos + i][yPos] = BattleshipCellType::SHIP;
            }
            else
            {
                board[xPos][yPos + i] = BattleshipCellType::SHIP;
            }
        }

        break;
    }

    GetLogger().tag("Battleships") << "Added ship of size " << size << " in " << n << " random attempts.";
}