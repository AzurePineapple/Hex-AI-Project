#include "Board.h"

Board::Board(int size, int ColourPairChoice)
{
    switch (ColourPairChoice)
    {
    case 1:
        activeColours[0] = blackColour;
        activeColours[1] = whiteColour;
        activeColoursGhosts[0] = blackColourGhost;
        activeColoursGhosts[1] = whiteColourGhost;
        break;
    case 2:
        activeColours[0] = redColour;
        activeColours[1] = blueColour;
        activeColoursGhosts[0] = redColourGhost;
        activeColoursGhosts[1] = blueColourGhost;
        break;
    case 3:
        activeColours[0] = greenColour;
        activeColours[1] = purpleColour;
        activeColoursGhosts[0] = greenColourGhost;
        activeColoursGhosts[1] = purpleColourGhost;
        break;
    case 4:
        activeColours[0] = lightBlueColour;
        activeColours[1] = orangeColour;
        activeColoursGhosts[0] = lightBlueColourGhost;
        activeColoursGhosts[1] = orangeColourGhost;
        break;
    case 5:
        activeColours[0] = regalBlueColour;
        activeColours[1] = yellowColour;
        activeColoursGhosts[0] = regalBlueColourGhost;
        activeColoursGhosts[1] = yellowColourGhost;
        break;
    default:
        activeColours[0] = blackColour;
        activeColours[1] = whiteColour;
        activeColoursGhosts[0] = blackColourGhost;
        activeColoursGhosts[1] = whiteColourGhost;
        break;
    }

    boardSize = size;
    requiredWidth = boardSize + floor(boardSize / 2);
    boardMatrix.resize(boardSize, std::vector<int>(boardSize, 0));

    // Insert position offsets into map
    offsets.insert({3, {198, 40}});
    offsets.insert({5, {203, 72}});
    offsets.insert({7, {88, 10}});
    offsets.insert({9, {89, 17}});
    offsets.insert({11, {86, 23}});
    offsets.insert({13, {97, 29}});

    for (int i = 0; i < boardSize; i++)
    {
        for (int j = 0; j < boardSize; j++)
        {
            // Creating an index map, which will allow conversion between indeces of 1d and 2d array
            indexMap.push_back({i, j});
        }
    }

    createDisjointSets();
}

void Board::createDisjointSets()
{
    // Create virtual nodes for detecting win state
    topNode = std::make_pair(-1, 0);
    bottomNode = std::make_pair(boardSize, 0);
    leftNode = std::make_pair(0, -1);
    rightNode = std::make_pair(0, boardSize);

    // Create a disjoint set for each player
    std::vector<std::pair<int, int>> blackIndexMap = indexMap;
    std::vector<std::pair<int, int>> whiteIndexMap = indexMap;

    // Add virtual nodes to each set
    blackIndexMap.insert(blackIndexMap.end(), {topNode, bottomNode});
    whiteIndexMap.insert(whiteIndexMap.end(), {leftNode, rightNode});

    // Somehow got foward declaration of objects to work (emptied default constructor of DS and moved that functionality to populateSet)
    dsBlack.populateSet(blackIndexMap);
    dsWhite.populateSet(whiteIndexMap);
    // Link the virtual nodes to the sides of the board
    for (int i = 0; i < boardSize; i++)
    {
        dsBlack.unionBySize(std::make_pair(0, i), topNode);
        dsBlack.unionBySize(std::make_pair(boardSize - 1, i), bottomNode);
        dsWhite.unionBySize(std::make_pair(i, 0), leftNode);
        dsWhite.unionBySize(std::make_pair(i, boardSize - 1), rightNode);
    }
}

void Board::showBoard()
{
    std::cout << std::string(2 * boardSize + 2, '-') << std::endl;
    for (int i = 0; i < boardSize; i++)
    {
        std::cout << std::string(i, ' ');
        std::cout << "\\ ";
        for (int j = 0; j < boardSize; j++)
        {
            std::cout << boardMatrix[i][j] << " ";
        }
        std::cout << "\\" << std::endl;
    }
    std::cout << std::string(boardSize, ' ');
    std::cout << std::string(2 * boardSize + 2, '-') << std::endl;
}

bool Board::placePiece(std::string player, int i, int j)
{
    if (0 <= i && i < boardSize && 0 <= j && j < boardSize)
    {
        // Populate the vector with neighbouring cells to selected cell
        createNeighbours(i, j);

        if (tiles[getOneDIndex(i, j)].getContents() == "empty")
        {
            if (player == "black")
            {
                boardMatrix[i][j] = 1;
                tiles[getOneDIndex(i, j)].setContents(1);
                connectToNeighbours(i, j, 1);
            }

            else if (player == "white")
            {
                boardMatrix[i][j] = 2;
                tiles[getOneDIndex(i, j)].setContents(2);
                connectToNeighbours(i, j, 2);
            }

            bool gameOver = detectWinner();
            return gameOver;
        }
        return false;
    }
    else
    {
        std::cout << "Passed coordinates are out of bounds, recieved: (" << i << ", " << j << ")" << std::endl;
    }

    return false;
}

void Board::createNeighbours(int i, int j)
{
    neighbours.clear();
    neighbours.push_back(std::make_pair(i - 1, j));
    neighbours.push_back(std::make_pair(i - 1, j + 1));
    neighbours.push_back(std::make_pair(i, j - 1));
    neighbours.push_back(std::make_pair(i, j + 1));
    neighbours.push_back(std::make_pair(i + 1, j - 1));
    neighbours.push_back(std::make_pair(i + 1, j));
}

int Board::getContents(int i, int j)
{
    return boardMatrix[i][j];
}

Hexagon *Board::getTileObj(int i, int j)
{
    if (getOneDIndex(i, j) == -1)
    {

        std::cout << "Tile could not be found, returning first tile" << std::endl;
        return &tiles[0];
    }
    else
    {

        return &tiles[getOneDIndex(i, j)];
    }
}

int Board::getOneDIndex(int i, int j)
{

    std::pair<int, int> searchPair = {i, j};
    auto iterator = std::find(indexMap.begin(), indexMap.end(), searchPair);
    if (iterator != indexMap.end())
    {
        int index = iterator - indexMap.begin();
        return index;
    }
    else
    {
        return -1;
    }
}

void Board::createBoard(int screenwidth)
{

    int radius = screenwidth / (2 * requiredWidth);

    // Hacky way to reduce size of radius on smaller boards
    if (boardSize < 7)
    {
        radius = radius * 0.8;
    }

    for (int i = 0; i < boardSize; i++)
    {
        for (int j = 0; j < boardSize; j++)
        {
            // Coordinates are axial offset + j * horizontal offset + i * vertical offset.
            // Y coordinate requires no y axis offset within each row.
            Hexagon hex(offsets[boardSize][0] + radius * ((sqrt(3.0) * 0.5) + (j * (sqrt(3.0))) + i * (sqrt(3.0) * 0.5)), offsets[boardSize][1] + radius * (1 + (i * 1.5)), activeColours, activeColoursGhosts, radius);
            hex.setContents(getContents(i, j));

            // Add hexagon to 1D vector for access to bounding boxes
            tiles.push_back(hex);
        }
    }
}

void Board::drawBoard(SDL_Renderer *Renderer, int screenwidth)
{
    // Dark mode
    // SDL_SetRenderDrawColor(Renderer, 31, 31, 31, 255);

    // Light mode
    SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);

    SDL_RenderClear(Renderer);
    int radius = screenwidth / (2 * requiredWidth);
    drawBoardEdges(Renderer, boardSize, radius);

    for (auto &&hex : tiles)
    {
        hex.drawHexagon(Renderer);
    }
}

void Board::showIndexMap()
{
    for (auto &&i : indexMap)
    {
        std::cout << i.first << ", " << i.second << std::endl;
    }
}

std::vector<std::pair<int, int>> Board::getIndexMap()
{
    return indexMap;
}

void Board::drawBoardEdges(SDL_Renderer *Renderer, int boardSize, int radius)
{
    // Create two sets of vertices, both sets have the same positions, but different colours so SDL_RenderGeometry can change their colours
    // Colour 1 points
    SDL_Vertex BoardTopLeftCorner_COLOUR_1 = {{float(tiles[0].getCoords().first - (radius * tan(M_PI / 3.0))), float(tiles[0].getCoords().second - radius)}, activeColours[0], {1, 1}};
    SDL_Vertex BoardTopRightCorner_COLOUR_1 = {{float(tiles[boardSize - 1].getCoords().first + (radius * tan(M_PI / 6))), float(tiles[boardSize - 1].getCoords().second - radius)}, activeColours[0], {1, 1}};
    SDL_Vertex BoardBottomRightCorner_COLOUR_1 = {{float(tiles[getOneDIndex(boardSize - 1, boardSize - 1)].getCoords().first + (radius * tan(M_PI / 3.0))), float(tiles[getOneDIndex(boardSize - 1, boardSize - 1)].getCoords().second + radius)}, activeColours[0], {1, 1}};
    SDL_Vertex BoardBottomLeftCorner_COLOUR_1 = {{float(tiles[getOneDIndex(boardSize - 1, 0)].getCoords().first - (radius * tan(M_PI / 6.0))), float(tiles[getOneDIndex(boardSize - 1, 0)].getCoords().second + radius)}, activeColours[0], {1, 1}};

    SDL_Vertex TopLeftTileCenter_COLOUR_1 = {{float(tiles[0].getCoords().first), float(tiles[0].getCoords().second)}, activeColours[0], {1, 1}};
    SDL_Vertex TopRightTileCenter_COLOUR_1 = {{float(tiles[boardSize - 1].getCoords().first), float(tiles[boardSize - 1].getCoords().second)}, activeColours[0], {1, 1}};
    SDL_Vertex BottomRightTileCenter_COLOUR_1 = {{float(tiles[getOneDIndex(boardSize - 1, boardSize - 1)].getCoords().first), float(tiles[getOneDIndex(boardSize - 1, boardSize - 1)].getCoords().second)}, activeColours[0], {1, 1}};
    SDL_Vertex BottomLeftTileCenter_COLOUR_1 = {{float(tiles[getOneDIndex(boardSize - 1, 0)].getCoords().first), float(tiles[getOneDIndex(boardSize - 1, 0)].getCoords().second)}, activeColours[0], {1, 1}};

    // Colour 2 points
    SDL_Vertex BoardTopLeftCorner_COLOUR_2 = {{float(tiles[0].getCoords().first - (radius * tan(M_PI / 3.0))), float(tiles[0].getCoords().second - radius)}, activeColours[1], {1, 1}};
    SDL_Vertex BoardTopRightCorner_COLOUR_2 = {{float(tiles[boardSize - 1].getCoords().first + (radius * tan(M_PI / 6))), float(tiles[boardSize - 1].getCoords().second - radius)}, activeColours[1], {1, 1}};
    SDL_Vertex BoardBottomRightCorner_COLOUR_2 = {{float(tiles[getOneDIndex(boardSize - 1, boardSize - 1)].getCoords().first + (radius * tan(M_PI / 3.0))), float(tiles[getOneDIndex(boardSize - 1, boardSize - 1)].getCoords().second + radius)}, activeColours[1], {1, 1}};
    SDL_Vertex BoardBottomLeftCorner_COLOUR_2 = {{float(tiles[getOneDIndex(boardSize - 1, 0)].getCoords().first - (radius * tan(M_PI / 6.0))), float(tiles[getOneDIndex(boardSize - 1, 0)].getCoords().second + radius)}, activeColours[1], {1, 1}};

    SDL_Vertex TopLeftTileCenter_COLOUR_2 = {{float(tiles[0].getCoords().first), float(tiles[0].getCoords().second)}, activeColours[1], {1, 1}};
    SDL_Vertex TopRightTileCenter_COLOUR_2 = {{float(tiles[boardSize - 1].getCoords().first), float(tiles[boardSize - 1].getCoords().second)}, activeColours[1], {1, 1}};
    SDL_Vertex BottomRightTileCenter_COLOUR_2 = {{float(tiles[getOneDIndex(boardSize - 1, boardSize - 1)].getCoords().first), float(tiles[getOneDIndex(boardSize - 1, boardSize - 1)].getCoords().second)}, activeColours[1], {1, 1}};
    SDL_Vertex BottomLeftTileCenter_COLOUR_2 = {{float(tiles[getOneDIndex(boardSize - 1, 0)].getCoords().first), float(tiles[getOneDIndex(boardSize - 1, 0)].getCoords().second)}, activeColours[1], {1, 1}};

    int indexList[6] = {0, 1, 2, 2, 3, 0};

    // Top Border
    SDL_Vertex TOPboxVertices[4] = {BoardTopLeftCorner_COLOUR_1, BoardTopRightCorner_COLOUR_1, TopRightTileCenter_COLOUR_1, TopLeftTileCenter_COLOUR_1};
    SDL_RenderGeometry(Renderer, NULL, TOPboxVertices, 4, indexList, 6);

    // Bottom Border
    SDL_Vertex BOTboxVertices[4] = {BottomLeftTileCenter_COLOUR_1, BottomRightTileCenter_COLOUR_1, BoardBottomRightCorner_COLOUR_1, BoardBottomLeftCorner_COLOUR_1};
    SDL_RenderGeometry(Renderer, NULL, BOTboxVertices, 4, indexList, 6);

    // Left Border
    SDL_Vertex LEFTboxVertices[4] = {BoardTopLeftCorner_COLOUR_2, TopLeftTileCenter_COLOUR_2, BottomLeftTileCenter_COLOUR_2, BoardBottomLeftCorner_COLOUR_2};
    SDL_RenderGeometry(Renderer, NULL, LEFTboxVertices, 4, indexList, 6);

    // Right Border
    SDL_Vertex RIGHTboxVertices[4] = {TopRightTileCenter_COLOUR_2, BoardTopRightCorner_COLOUR_2, BoardBottomRightCorner_COLOUR_2, BottomRightTileCenter_COLOUR_2};
    SDL_RenderGeometry(Renderer, NULL, RIGHTboxVertices, 4, indexList, 6);

    // Draw outlines
    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
    SDL_RenderDrawLine(Renderer, BoardTopLeftCorner_COLOUR_1.position.x, BoardTopLeftCorner_COLOUR_1.position.y, BoardTopRightCorner_COLOUR_1.position.x, BoardTopRightCorner_COLOUR_1.position.y);
    SDL_RenderDrawLine(Renderer, BoardTopRightCorner_COLOUR_1.position.x, BoardTopRightCorner_COLOUR_1.position.y, BoardBottomRightCorner_COLOUR_1.position.x, BoardBottomRightCorner_COLOUR_1.position.y);
    SDL_RenderDrawLine(Renderer, BoardTopLeftCorner_COLOUR_1.position.x, BoardTopLeftCorner_COLOUR_1.position.y, BoardBottomLeftCorner_COLOUR_1.position.x, BoardBottomLeftCorner_COLOUR_1.position.y);
    SDL_RenderDrawLine(Renderer, BoardBottomLeftCorner_COLOUR_1.position.x, BoardBottomLeftCorner_COLOUR_1.position.y, BoardBottomRightCorner_COLOUR_1.position.x, BoardBottomRightCorner_COLOUR_1.position.y);
}

void Board::connectToNeighbours(int i, int j, int colourCode)
{
    for (auto &&neiPair : neighbours)
    {
        // Check that neighbour tile is within board bounds, and that its contents matches contents of cell to be played (colourCode)
        if (0 <= neiPair.first && neiPair.first < boardSize && 0 <= neiPair.second && neiPair.second < boardSize && colourCode == boardMatrix[neiPair.first][neiPair.second])
        {
            if (colourCode == 1)
            {
                dsBlack.unionBySize(neiPair, std::make_pair(i, j));
            }
            else
            {
                dsWhite.unionBySize(neiPair, std::make_pair(i, j));
            }
        }
    }
}

bool Board::detectWinner()
{
    if (dsBlack.find(topNode) == dsBlack.find(bottomNode))
    {
        std::cout << "Player 1 Wins!" << std::endl;
        std::vector<std::pair<int, int>> winningPath;
        for (auto &&i : indexMap)
        {
            if (dsBlack.find(i) == dsBlack.find(topNode))
            {
                winningPath.push_back(i);
                // Cross check this with board contents to elimate empty tiles along edges of the board
            }
        }

        // for (auto &&i : winningPath)
        // {
        //     if (boardMatrix[i.first][i.second] == 1)
        //     {
        //         std::cout << i.first << ", " << i.second << std::endl;
        //     }
        // }

        return true;
    }
    else if (dsWhite.find(leftNode) == dsWhite.find(rightNode))
    {
        std::cout << "Player 2 Wins!" << std::endl;
        std::vector<std::pair<int, int>> winningPath;
        for (auto &&i : indexMap)
        {
            if (dsWhite.find(i) == dsWhite.find(topNode))
            {
                winningPath.push_back(i);
            }
        }

        // for (auto &&i : winningPath)
        // {
        //     if (boardMatrix[i.first][i.second] == 2)
        //     {
        //         std::cout << i.first << ", " << i.second << std::endl;
        //     }
        // }

        return true;
    }
    return false;
}

std::vector<std::vector<int>> Board::getBoardMatrix()
{
    return boardMatrix;
}

std::vector<std::pair<int, int>> Board::getEmptyTiles()
{
    std::vector<std::pair<int, int>> emptyTiles;
    std::pair<int, int> p;

    for (int i = 0; i < boardSize; i++)
    {
        for (int j = 0; j < boardSize; j++)
        {
            if (boardMatrix[i][j] == 0)
            {
                p = std::make_pair(i, j);
                emptyTiles.push_back(p);
            }
        }
    }

    return emptyTiles;
}

void Board::boardSwap()
{
    // Create copies of board matrix and tile storage
    std::vector<std::vector<int>> boardMatrix2;
    boardMatrix2.resize(boardSize, std::vector<int>(boardSize, 0));

    // Clear blacks DS, as black always goes first
    dsBlack.wipeDS();
    // Clear whites DS, cos I haven't limited when this can be called yet
    dsWhite.wipeDS();
    createDisjointSets();

    // Sets the contents of every tile on the board to empty (this is the visual representation)
    for (auto &&hex : tiles)
    {
        hex.setContents(0);
    }

    for (size_t i = 0; i < boardMatrix.size(); i++)
    {
        for (size_t j = 0; j < boardMatrix.size(); j++)
        {
            // Mirror old matrix (boardMatrix) along diagonal into new matrix (boardMatrix2)
            boardMatrix2[i][j] = boardMatrix[j][i];

            // Create neighbours for things
            createNeighbours(i, j);

            // Update stuff based on new board, inverting the piece colour following "swap pieces" implementation of rule (https://www.hexwiki.net/index.php/Swap_rule)
            switch (boardMatrix2[i][j])
            {
            case 1:
                boardMatrix2[i][j] = 2;
                // Sets the visual representation
                tiles[getOneDIndex(i, j)].setContents(2);
                // Sets the DS stuff
                connectToNeighbours(i, j, 2);
                break;

            case 2:
                boardMatrix2[i][j] = 1;
                tiles[getOneDIndex(i, j)].setContents(1);
                connectToNeighbours(i, j, 1);
                break;
            default:
                break;
            }
        }
    }
    boardMatrix = boardMatrix2;
    // showBoard();
}

int Board::getSize()
{
    return boardSize;
}