#include <Game/BattleshipsMinigame.hpp>
#include <Common/Logger.hpp>

const int BATTLESHIP_CELL_SIZE = 64;
const int BATTLESHIP_BOARD_RENDER_SIZE = BATTLESHIP_BOARD_SIZE * BATTLESHIP_CELL_SIZE;
const int BATTLESHIP_BOARDS_MARGIN = 64;
;

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

    m_gridHorizontalLine.setFillColor(sf::Color::White);
    m_gridVerticalLine.setFillColor(sf::Color::White);

    m_gridHorizontalLine.setSize(
        sf::Vector2f(
            BATTLESHIP_BOARD_RENDER_SIZE,
            1));

    m_gridVerticalLine.setSize(
        sf::Vector2f(
            1,
            BATTLESHIP_BOARD_RENDER_SIZE));

    m_boardBackground.setFillColor(
        sf::Color(63, 81, 106, 100));
    m_boardBackground.setSize(
        sf::Vector2f(BATTLESHIP_BOARD_RENDER_SIZE, BATTLESHIP_BOARD_RENDER_SIZE));

    m_turnText.setFont(
        m_context.assetManager->getFont("main"));

    m_turnText.setCharacterSize(32);
    m_turnText.setFillColor(sf::Color::White);

    m_hoverTile.setSize(
        sf::Vector2f(BATTLESHIP_CELL_SIZE, BATTLESHIP_CELL_SIZE));
    m_hoverTile.setFillColor(
        sf::Color(255, 255, 255, 100));
}

void BattleshipsMinigame::render()
{
    m_context.window.draw(m_bg);

    sf::Vector2f enemyBoardStartPosition = sf::Vector2f(
        m_context.window.getSize().x / 2 + BATTLESHIP_BOARDS_MARGIN,
        m_context.window.getSize().y / 2 - BATTLESHIP_BOARD_RENDER_SIZE / 2);

    renderBoard(
        m_playerBoard,
        sf::Vector2f(
            m_context.window.getSize().x / 2 - BATTLESHIP_BOARD_RENDER_SIZE - BATTLESHIP_BOARDS_MARGIN,
            m_context.window.getSize().y / 2 - BATTLESHIP_BOARD_RENDER_SIZE / 2),
        false);

    renderBoard(
        m_enemyBoard,
        enemyBoardStartPosition,
        true);

    m_context.window.draw(m_turnText);

    sf::Vector2f mousePos = m_context.window.mapPixelToCoords(sf::Mouse::getPosition(m_context.window));

    if (m_isPlayerTurn && m_turnState == BattleshipTurnState::THINKING && mousePos.x > enemyBoardStartPosition.x &&
        mousePos.x < enemyBoardStartPosition.x + BATTLESHIP_BOARD_RENDER_SIZE &&
        mousePos.y > enemyBoardStartPosition.y &&
        mousePos.y < enemyBoardStartPosition.y + BATTLESHIP_BOARD_RENDER_SIZE)
    {
        int x = (mousePos.x - enemyBoardStartPosition.x) / BATTLESHIP_CELL_SIZE;
        int y = (mousePos.y - enemyBoardStartPosition.y) / BATTLESHIP_CELL_SIZE;

        m_hoverTile.setPosition(
            enemyBoardStartPosition.x + x * BATTLESHIP_CELL_SIZE,
            enemyBoardStartPosition.y + y * BATTLESHIP_CELL_SIZE);

        m_context.window.draw(m_hoverTile);
    }
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
            printf("%d ", m_enemyBoard[i][j]);
        }
        printf("\n");
    }

    m_turnState = BattleshipTurnState::THINKING;
    m_isPlayerTurn = true;
    setTurnStatusText(L"Ваш хід! Оберіть координати для пострілу");
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

bool BattleshipsMinigame::shoot(BattleshipBoard &board, int x, int y)
{
}

void BattleshipsMinigame::renderBoard(BattleshipBoard &board, sf::Vector2f startPos, bool hideShips)
{
    m_boardBackground.setPosition(startPos);
    m_context.window.draw(m_boardBackground);

    for (int i = 0; i < BATTLESHIP_BOARD_SIZE + 1; i++)
    {
        m_gridHorizontalLine.setPosition(
            startPos.x,
            startPos.y + i * BATTLESHIP_CELL_SIZE);

        m_gridVerticalLine.setPosition(
            startPos.x + i * BATTLESHIP_CELL_SIZE,
            startPos.y);

        m_context.window.draw(m_gridHorizontalLine);
        m_context.window.draw(m_gridVerticalLine);
    }
}

void BattleshipsMinigame::setTurnStatusText(std::wstring str)
{
    m_turnText.setString(str);
    m_turnText.setPosition(
        sf::Vector2f(
            m_context.window.getSize().x / 2 - m_turnText.getLocalBounds().width / 2,
            m_context.window.getSize().y - m_turnText.getLocalBounds().height / 2 - 128));
}