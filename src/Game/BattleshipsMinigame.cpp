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
    m_music.setVolume(50);

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

    m_enemyBoardStartPosition = sf::Vector2f(
        m_context.window.getSize().x / 2 + BATTLESHIP_BOARDS_MARGIN,
        m_context.window.getSize().y / 2 - BATTLESHIP_BOARD_RENDER_SIZE / 2);

    m_missSound.setBuffer(
        m_context.assetManager->getSound("splash"));

    m_hitSound.setBuffer(
        m_context.assetManager->getSound("ship_hit"));

    m_shootSound.setBuffer(
        m_context.assetManager->getSound("mortar"));

    m_missIcon.setTexture(
        m_context.assetManager->getTexture("cross"));

    m_shipIcon.setTexture(
        m_context.assetManager->getTexture("ship"));

    m_fireAnimation.setSpriteSheet(
        m_context.assetManager->getTexture("fire"));
    m_fireAnimation.addFrame(
        sf::IntRect(0, 0, 32, 32));
    m_fireAnimation.addFrame(
        sf::IntRect(32, 0, 32, 32));
    m_fireAnimation.addFrame(
        sf::IntRect(64, 0, 32, 32));

    m_fireIcon.setAnimation(m_fireAnimation);

    m_sunkShipIcon.setTexture(
        m_context.assetManager->getTexture("sunk_ship"));

    m_debugEnemyShips = false;
    m_lastEnemyHitWasSuccessful = false;
}

void BattleshipsMinigame::render()
{
    m_context.window.draw(m_bg);

    renderBoard(
        m_playerBoard,
        sf::Vector2f(
            m_context.window.getSize().x / 2 - BATTLESHIP_BOARD_RENDER_SIZE - BATTLESHIP_BOARDS_MARGIN,
            m_context.window.getSize().y / 2 - BATTLESHIP_BOARD_RENDER_SIZE / 2),
        false);

    renderBoard(
        m_enemyBoard,
        m_enemyBoardStartPosition,
        !m_debugEnemyShips);

    m_context.window.draw(m_turnText);

    sf::Vector2f mousePos = m_context.window.mapPixelToCoords(sf::Mouse::getPosition(m_context.window));

    if (m_isPlayerTurn && m_turnState == BattleshipTurnState::THINKING)
    {
        sf::Vector2i hoverCellPos = tryGetCellAtMouse(m_enemyBoardStartPosition);

        if (hoverCellPos.x != -1)
        {
            m_hoverTile.setPosition(
                m_enemyBoardStartPosition.x + hoverCellPos.x * BATTLESHIP_CELL_SIZE,
                m_enemyBoardStartPosition.y + hoverCellPos.y * BATTLESHIP_CELL_SIZE);

            m_context.window.draw(m_hoverTile);
        }
    }
}

void BattleshipsMinigame::checkDamage()
{
    BattleshipBoard &targetBoard = m_isPlayerTurn ? m_enemyBoard : m_playerBoard;

    auto cellType = targetBoard[m_targetShootPos.x][m_targetShootPos.y];

    bool wasHit = false;

    if (cellType == BattleshipCellType::SHIP)
    {
        targetBoard[m_targetShootPos.x][m_targetShootPos.y] = BattleshipCellType::HIT;
        m_hitSound.play();

        if (m_isPlayerTurn)
        {
            setTurnStatusText(L"Ви вражаєте ворожий корабель!");
        }
        else
        {
            setTurnStatusText(L"Святослав попадає в ваш корабель!");
            m_lastEnemyHitWasSuccessful = true;
            m_lastEnemyHitPosition = m_targetShootPos;
        }

        wasHit = true;
    }
    else
    {
        setTurnStatusText(L"Мимо!");
        m_missSound.play();
        targetBoard[m_targetShootPos.x][m_targetShootPos.y] = BattleshipCellType::MISS;
    }

    if (!wasHit)
        return;

    std::vector<sf::Vector2i> cellsToCheck;
    std::vector<sf::Vector2i> cellsToMakeMiss;
    std::vector<sf::Vector2i> cellsVisited;
    std::vector<sf::Vector2i> cellsToMakeSunk;
    cellsToCheck.reserve(16);
    cellsToMakeMiss.reserve(16);
    cellsVisited.reserve(16);
    cellsToMakeSunk.reserve(4);

    bool shipDestroyed = true;

    cellsToCheck.push_back(sf::Vector2i(m_targetShootPos.x, m_targetShootPos.y));

    while (!cellsToCheck.empty())
    {
        auto cell = cellsToCheck.back();
        cellsToCheck.pop_back();

        auto cellType = targetBoard[cell.x][cell.y];

        if (cell.x < 0 || cell.x >= BATTLESHIP_BOARD_SIZE ||
            cell.y < 0 || cell.y >= BATTLESHIP_BOARD_SIZE)
        {
            continue;
        }

        if (std::find(cellsVisited.begin(), cellsVisited.end(), cell) != cellsVisited.end())
        {
            continue;
        }

        cellsVisited.push_back(cell);

        if (cellType == BattleshipCellType::SHIP)
        {
            shipDestroyed = false;
            break;
        }

        if (cellType == BattleshipCellType::HIT)
        {
            cellsToCheck.push_back(sf::Vector2i(cell.x + 1, cell.y));
            cellsToCheck.push_back(sf::Vector2i(cell.x - 1, cell.y));
            cellsToCheck.push_back(sf::Vector2i(cell.x, cell.y - 1));
            cellsToCheck.push_back(sf::Vector2i(cell.x, cell.y + 1));
            cellsToMakeSunk.push_back(cell);
            continue;
        }

        cellsToMakeMiss.push_back(cell);
    }

    if (shipDestroyed)
    {
        if (m_isPlayerTurn)
        {
            setTurnStatusText(L"Ви знищили ворожий корабель!");
        }
        else
        {
            setTurnStatusText(L"Святослав знищив ваш корабель!");
            m_lastEnemyHitWasSuccessful = false;
        }

        for (auto cell : cellsToMakeMiss)
        {
            targetBoard[cell.x][cell.y] = BattleshipCellType::MISS;
        }

        for (auto cell : cellsToMakeSunk)
        {
            targetBoard[cell.x][cell.y] = BattleshipCellType::SUNK;
        }
    }

    for (int i = 0; i < BATTLESHIP_BOARD_SIZE; i++)
    {
        for (int j = 0; j < BATTLESHIP_BOARD_SIZE; j++)
        {
            if (targetBoard[i][j] == BattleshipCellType::SHIP)
            {
                return;
            }
        }
    }

    if (m_isPlayerTurn)
    {
        setTurnStatusText(L"Ви перемогли!");
    }
    else
    {
        setTurnStatusText(L"Святослав переміг!");
    }

    m_hasPlayerWon = m_isPlayerTurn;
    m_turnState = BattleshipTurnState::FINISHED;
    m_turnClock.restart();
}

void BattleshipsMinigame::update(float delta)
{
    if (m_turnState == BattleshipTurnState::SHOOTING)
    {
        if (m_turnClock.getElapsedTime().asSeconds() > 3.0f)
        {
            m_turnState = BattleshipTurnState::OUTCOME;
            m_turnClock.restart();

            checkDamage();
        }
    }
    else if (m_turnState == BattleshipTurnState::OUTCOME)
    {
        if (m_turnClock.getElapsedTime().asSeconds() > 2.5f)
        {
            m_turnState = BattleshipTurnState::THINKING;
            m_isPlayerTurn = !m_isPlayerTurn;
            m_turnClock.restart();

            if (m_isPlayerTurn)
            {
                setTurnStatusText(L"Ваш хід! Оберіть координати для пострілу");
            }
            else
            {
                setTurnStatusText(L"Святослав обирає ціль для пострілу...");
            }
        }
    }
    else if (m_turnState == BattleshipTurnState::THINKING)
    {
        if (!m_isPlayerTurn && m_turnClock.getElapsedTime().asSeconds() > 1.5f)
        {
            executeEnemyTurn();
        }
    }
    else if (m_turnState == BattleshipTurnState::FINISHED)
    {
        if (m_turnClock.getElapsedTime().asSeconds() > 2.0f)
        {
            m_finished = true;
            m_music.stop();
        }
    }

    m_fireIcon.update(sf::seconds(delta));
}

void BattleshipsMinigame::start()
{
    prepareBoard(m_playerBoard);
    prepareBoard(m_enemyBoard);

    m_turnState = BattleshipTurnState::THINKING;
    m_isPlayerTurn = true;
    setTurnStatusText(L"Ваш хід! Оберіть координати для пострілу");
}

void BattleshipsMinigame::input(sf::Event &ev)
{
    if (ev.type == sf::Event::MouseButtonReleased)
    {
        if (m_turnState == BattleshipTurnState::THINKING && ev.mouseButton.button == sf::Mouse::Left)
        {
            auto cellPos = tryGetCellAtMouse(m_enemyBoardStartPosition);

            if (cellPos.x != -1)
            {
                shoot(
                    m_enemyBoard,
                    cellPos.x,
                    cellPos.y);
            }
        }
    }

    if (ev.type == sf::Event::KeyReleased)
    {
        if (ev.key.code == sf::Keyboard::P)
        {
            m_debugEnemyShips = !m_debugEnemyShips;
        }
    }
}

bool BattleshipsMinigame::hasUserWon()
{
    return m_hasPlayerWon;
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

    // 1x4
    addShip(4, board);

    // 3x2
    addShip(2, board);
    addShip(2, board);
    addShip(2, board);

    // 2x3
    addShip(3, board);
    addShip(3, board);

    // 4x1
    addShip(1, board);
    addShip(1, board);
    addShip(1, board);
    addShip(1, board);
}

void BattleshipsMinigame::addShip(int size, BattleshipBoard &board)
{
    std::uniform_int_distribution<> boardDistr(0, BATTLESHIP_BOARD_SIZE);
    std::uniform_int_distribution<> dirDistr(0, 1);
    int n = 0;

    std::vector<sf::Vector2i> possiblePositions;
    possiblePositions.reserve(BATTLESHIP_BOARD_SIZE * BATTLESHIP_BOARD_SIZE);

    for (int x = 0; x < BATTLESHIP_BOARD_SIZE; x++)
    {
        for (int y = 0; y < BATTLESHIP_BOARD_SIZE; y++)
        {
            if (board[x][y] == BattleshipCellType::EMPTY)
                possiblePositions.push_back(sf::Vector2i(x, y));
        }
    }

    std::shuffle(possiblePositions.begin(), possiblePositions.end(), m_rndGen);

    int lastDir = 0;

    while (!possiblePositions.empty())
    {
        auto pos = possiblePositions.back();
        possiblePositions.pop_back();

        int ndir = dirDistr(m_rndGen);

        if (tryAddingShipAtPosition(
                board,
                size,
                pos.x,
                pos.y,
                ndir ? 0 : 1))
        {
            break;
        }

        if (tryAddingShipAtPosition(
                board,
                size,
                pos.x,
                pos.y,
                ndir ? 1 : 0))
            break;

        n++;
    }

    // int n = 0;

    // int xPos = boardDistr(m_rndGen);
    // int yPos = boardDistr(m_rndGen);
    // int dir = dirDistr(m_rndGen);

    // bool lastDirWasOkay = false;

    // while (n < BATTLESHIP_BOARD_SIZE * BATTLESHIP_BOARD_SIZE)
    // {

    //     bool occupied = false;

    //     for (int i = 0; i < size; i++)
    //     {
    //         int cxPos = dir ? xPos + i : xPos;
    //         int cyPos = dir ? yPos : yPos + i;

    //         if (cxPos < 0 || cxPos >= BATTLESHIP_BOARD_SIZE ||
    //             cyPos < 0 || cyPos >= BATTLESHIP_BOARD_SIZE)
    //         {
    //             continue;
    //         }

    //         if (board[cxPos][cyPos] != BattleshipCellType::EMPTY)
    //         {
    //             occupied = true;
    //             break;
    //         }

    //         if (cxPos + 1 < BATTLESHIP_BOARD_SIZE && board[cxPos + 1][cyPos] != BattleshipCellType::EMPTY)
    //         {
    //             occupied = true;
    //             break;
    //         }

    //         if (cxPos - 1 >= 0 && board[cxPos - 1][cyPos] != BattleshipCellType::EMPTY)
    //         {
    //             occupied = true;
    //             break;
    //         }

    //         if (cyPos - 1 >= 0 && board[cxPos][cyPos - 1] != BattleshipCellType::EMPTY)
    //         {
    //             occupied = true;
    //             break;
    //         }

    //         if (cyPos + 1 < BATTLESHIP_BOARD_SIZE && board[cxPos][cyPos + 1] != BattleshipCellType::EMPTY)
    //         {
    //             occupied = true;
    //             break;
    //         }

    //         if (cyPos - 1 >= 0 && cxPos - 1 >= 0 && board[cxPos - 1][cyPos - 1] != BattleshipCellType::EMPTY)
    //         {
    //             occupied = true;
    //             break;
    //         }

    //         if (cyPos + 1 < BATTLESHIP_BOARD_SIZE && cxPos - 1 >= 0 && board[cxPos - 1][cyPos + 1] != BattleshipCellType::EMPTY)
    //         {
    //             occupied = true;
    //             break;
    //         }

    //         if (cyPos - 1 >= 0 && cxPos + 1 < BATTLESHIP_BOARD_SIZE && board[cxPos + 1][cyPos - 1] != BattleshipCellType::EMPTY)
    //         {
    //             occupied = true;
    //             break;
    //         }

    //         if (cyPos + 1 < BATTLESHIP_BOARD_SIZE && cxPos + 1 < BATTLESHIP_BOARD_SIZE && board[cxPos + 1][cyPos + 1] != BattleshipCellType::EMPTY)
    //         {
    //             occupied = true;
    //             break;
    //         }
    //     }

    //     if (occupied)
    //     {
    //         if (!lastDirWasOkay)
    //         {
    //             dir = !dir;
    //             lastDirWasOkay = true;
    //             continue;
    //         }

    //         xPos++;
    //         yPos++;

    //         if (xPos > BATTLESHIP_BOARD_SIZE - 1)
    //         {
    //             xPos = 0;
    //             yPos++;
    //         }

    //         if (yPos > BATTLESHIP_BOARD_SIZE - 1)
    //         {
    //             yPos = 0;
    //             xPos = 0;
    //         }
    //         lastDirWasOkay = false;
    //         n++;
    //         continue;
    //     }

    //     for (int i = 0; i < size; i++)
    //     {
    //         if (dir == 1)
    //         {
    //             board[xPos + i][yPos] = BattleshipCellType::SHIP;
    //         }
    //         else
    //         {
    //             board[xPos][yPos + i] = BattleshipCellType::SHIP;
    //         }
    //     }

    //     break;
    // }

    GetLogger().tag("Battleships") << "Added ship of size " << size << " in " << n << " random attempts.";
}

bool BattleshipsMinigame::shoot(BattleshipBoard &board, int x, int y)
{
    if (x < 0 || y < 0 || x > BATTLESHIP_BOARD_SIZE - 1 || y > BATTLESHIP_BOARD_SIZE - 1)
    {
        return false;
    }

    if (board[x][y] == BattleshipCellType::MISS || board[x][y] == BattleshipCellType::HIT || board[x][y] == BattleshipCellType::SUNK)
    {
        return false;
    }

    m_shootSound.play();

    m_targetShootPos = sf::Vector2i(x, y);

    if (m_isPlayerTurn)
    {
        setTurnStatusText(L"Гравець здійснює постріл по квадрату " + std::to_wstring(x) + L", " + std::to_wstring(y));
    }
    else
    {
        setTurnStatusText(L"Cвятослав здійснює постріл по квадрату " + std::to_wstring(x) + L", " + std::to_wstring(y));
    }

    m_turnClock.restart();
    m_turnState = BattleshipTurnState::SHOOTING;

    return true;
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

    for (int i = 0; i < BATTLESHIP_BOARD_SIZE; i++)
    {
        for (int j = 0; j < BATTLESHIP_BOARD_SIZE; j++)
        {
            sf::Vector2f cellRenderPos = sf::Vector2f(
                startPos.x + i * BATTLESHIP_CELL_SIZE,
                startPos.y + j * BATTLESHIP_CELL_SIZE);

            if (board[i][j] == BattleshipCellType::MISS)
            {
                m_missIcon.setPosition(cellRenderPos);
                m_context.window.draw(m_missIcon);
            }
            else if (board[i][j] == BattleshipCellType::SHIP && !hideShips)
            {
                m_shipIcon.setPosition(cellRenderPos);
                m_context.window.draw(m_shipIcon);
            }
            else if (board[i][j] == BattleshipCellType::HIT)
            {
                m_shipIcon.setPosition(cellRenderPos);
                m_context.window.draw(m_shipIcon);
                m_fireIcon.setPosition(cellRenderPos + sf::Vector2f(16, 16));
                m_context.window.draw(m_fireIcon);
            }
            else if (board[i][j] == BattleshipCellType::SUNK)
            {
                m_sunkShipIcon.setPosition(cellRenderPos);
                m_context.window.draw(m_sunkShipIcon);
            }
        }
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

sf::Vector2i BattleshipsMinigame::tryGetCellAtMouse(sf::Vector2f boardPos)
{
    sf::Vector2f mousePos = m_context.window.mapPixelToCoords(sf::Mouse::getPosition(m_context.window));

    if (
        mousePos.x > boardPos.x &&
        mousePos.x < boardPos.x + BATTLESHIP_BOARD_RENDER_SIZE &&
        mousePos.y > boardPos.y &&
        mousePos.y < boardPos.y + BATTLESHIP_BOARD_RENDER_SIZE)
    {
        int x = (mousePos.x - boardPos.x) / BATTLESHIP_CELL_SIZE;
        int y = (mousePos.y - boardPos.y) / BATTLESHIP_CELL_SIZE;

        return sf::Vector2i(x, y);
    }

    return sf::Vector2i(-1, -1);
}

void BattleshipsMinigame::executeEnemyTurn()
{
    std::uniform_int_distribution<> boardDistr(0, BATTLESHIP_BOARD_SIZE - 1);
    int tx = boardDistr(m_rndGen);
    int ty = boardDistr(m_rndGen);

    int n = BATTLESHIP_BOARD_SIZE * BATTLESHIP_BOARD_SIZE + 1;

    // For testing unfair AI!
    // for (int i = 0; i < BATTLESHIP_BOARD_SIZE; i++)
    // {
    //     for (int j = 0; j < BATTLESHIP_BOARD_SIZE; j++)
    //     {
    //         if (m_playerBoard[i][j] == BattleshipCellType::SHIP)
    //         {
    //             shoot(m_playerBoard, i, j);
    //             return;
    //         }
    //     }
    // }

    if (m_lastEnemyHitWasSuccessful)
    {
        int gx = m_lastEnemyHitPosition.x;
        int gy = m_lastEnemyHitPosition.y;

        std::vector<sf::Vector2i> possibleTargets = {
            sf::Vector2i(gx + 1, gy),
            sf::Vector2i(gx - 1, gy),
            sf::Vector2i(gx, gy + 1),
            sf::Vector2i(gx, gy - 1)};

        std::shuffle(
            possibleTargets.begin(),
            possibleTargets.end(),
            m_rndGen);

        for (int i = 0; i < 4; i++)
        {
            if (shoot(m_playerBoard, possibleTargets[i].x, possibleTargets[i].y))
            {
                return;
            }
        }
    }

    while (n > 0)
    {
        n--;

        if (!shoot(m_playerBoard, tx, ty))
        {
            tx++;
            if (tx >= BATTLESHIP_BOARD_SIZE)
            {
                ty++;
                tx = 0;
            }
            if (ty >= BATTLESHIP_BOARD_SIZE)
            {
                tx = 0;
                ty = 0;
            }
            continue;
        }
    }
}

bool BattleshipsMinigame::tryAddingShipAtPosition(BattleshipBoard &board, int size, int x, int y, int dir)
{
    sf::Vector2i addVector(
        dir ? 1 : 0,
        dir ? 0 : 1);

    sf::Vector2i pos(x, y);

    std::vector<sf::Vector2i> cellsToPlace;
    cellsToPlace.reserve(size);

    for (int sz = 0; sz < size; sz++)
    {
        if (pos.x < 0 || pos.x >= BATTLESHIP_BOARD_SIZE ||
            pos.y < 0 || pos.y >= BATTLESHIP_BOARD_SIZE)
        {
            return false;
        }

        sf::Vector2i neighboursToCheck[] = {
            sf::Vector2i(0, 0),
            sf::Vector2i(-1, -1),
            sf::Vector2i(0, -1),
            sf::Vector2i(1, -1),
            sf::Vector2i(-1, 0),
            sf::Vector2i(1, 0),
            sf::Vector2i(-1, 1),
            sf::Vector2i(0, 1),
            sf::Vector2i(1, 1)};

        GetLogger().tag("Battleships") << "Checking neighbours for " << pos.x << ", " << pos.y << " (ship size " << size << ", direction " << dir << ")";

        for (auto neighbour : neighboursToCheck)
        {
            auto newPos = pos + neighbour;

            if (newPos.x < 0 || newPos.x >= BATTLESHIP_BOARD_SIZE ||
                newPos.y < 0 || newPos.y >= BATTLESHIP_BOARD_SIZE)
            {
                continue;
            }

            if (board[newPos.x][newPos.y] != BattleshipCellType::EMPTY)
            {
                return false;
            }
        }

        cellsToPlace.push_back(pos);
        pos += addVector;
    }

    for (auto cell : cellsToPlace)
    {
        board[cell.x][cell.y] = BattleshipCellType::SHIP;
    }

    return true;
}