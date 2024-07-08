#include "A_star_approach.h"

A_star_approach::A_star_approach()
{
}

A_star_approach::~A_star_approach()
{
}

// Populates cells in the circuits with suitable values
// TODO: Add logic for patterns (big task)
void A_star_approach::updateCircuits(std::vector<std::vector<int>> boardMatrix, int colourCode)
{
    playerCode = colourCode;

    blackCircuit.resize(boardMatrix.size() + 2, std::vector<double>(boardMatrix.size() + 2, 0));
    whiteCircuit.resize(boardMatrix.size() + 2, std::vector<double>(boardMatrix.size() + 2, 0));

    // Creating parts of board that cannot be connected to black pieces

    // Corners
    blackCircuit[0][0] = std::numeric_limits<double>::max();
    blackCircuit[0][blackCircuit.size() - 1] = std::numeric_limits<double>::max();
    blackCircuit[blackCircuit.size() - 1][0] = std::numeric_limits<double>::max();
    blackCircuit[blackCircuit.size() - 1][blackCircuit.size() - 1] = std::numeric_limits<double>::max();

    // White's edges
    for (int i = 1; i < blackCircuit.size() - 1; i++)
    {
        blackCircuit[i][0] = std::numeric_limits<double>::max();
        blackCircuit[i][blackCircuit.size() - 1] = std::numeric_limits<double>::max();
    }

    // Creating parts of board that cannot be connected to white pieces

    // Corners
    whiteCircuit[0][0] = std::numeric_limits<double>::max();
    whiteCircuit[0][whiteCircuit.size() - 1] = std::numeric_limits<double>::max();
    whiteCircuit[whiteCircuit.size() - 1][0] = std::numeric_limits<double>::max();
    whiteCircuit[whiteCircuit.size() - 1][whiteCircuit.size() - 1] = std::numeric_limits<double>::max();

    // Blacks's edges
    for (int i = 1; i < whiteCircuit.size() - 1; i++)
    {
        whiteCircuit[0][i] = std::numeric_limits<double>::max();
        whiteCircuit[whiteCircuit.size() - 1][i] = std::numeric_limits<double>::max();
    }

    // Populate circuits using boardMatrix values
    // Representing difficulty to connect to each node
    // For use in pathfinding algorithm
    for (int i = 0; i < boardMatrix.size(); i++)
    {
        for (int j = 0; j < boardMatrix.size(); j++)
        {
            switch (boardMatrix[i][j])
            {
            case 0:
                blackCircuit[i + 1][j + 1] = 1;
                whiteCircuit[i + 1][j + 1] = 1;
                break;
            case 1:
                blackCircuit[i + 1][j + 1] = 0;
                whiteCircuit[i + 1][j + 1] = std::numeric_limits<double>::max(); // std::numeric_limits<int>::max();
                break;
            case 2:
                blackCircuit[i + 1][j + 1] = std::numeric_limits<double>::max(); // std::numeric_limits<int>::max();
                whiteCircuit[i + 1][j + 1] = 0;
                break;
            }
        }
    }
}

// Writes the current state of the circuits to the command line
void A_star_approach::showCircuits()
{
    std::cout << "Black Circuit" << std::endl;
    std::cout << "-------------------" << std::endl;
    for (int i = 0; i < blackCircuit.size(); i++)
    {
        std::cout << std::string(i, ' ');
        for (int j = 0; j < blackCircuit.size(); j++)
        {
            std::cout << blackCircuit[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "White Circuit" << std::endl;
    std::cout << "-------------------" << std::endl;
    for (int i = 0; i < whiteCircuit.size(); i++)
    {
        std::cout << std::string(i, ' ');
        for (int j = 0; j < whiteCircuit.size(); j++)
        {
            std::cout << whiteCircuit[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

// Checks if given cell is within sane bounds
bool A_star_approach::isCellValid(std::vector<std::vector<double>> playerCircuit, int i, int j)
{
    return (i >= 0) && (i < playerCircuit.size()) && (j >= 0) && (j < playerCircuit.size());
}

// Returns true if cell is occupied
bool A_star_approach::isCellOccupied(std::vector<std::vector<double>> playerCircuit, int i, int j)
{
    if (playerCircuit[i][j] > 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// Check if a given cell is the destination cell
bool A_star_approach::isDestination(int i, int j, std::pair<int, int> destination)
{
    if (i == destination.first && j == destination.second)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// Calculate heuristic value for use in A* algorithm
double A_star_approach::calculateHValue(int i, int j, std::pair<int, int> destination)
{
    // Original plan was to use Manhattan distance, as board is treated as square board
    // However since we are trying to connect an entire side, not a specific point, we can use the distance from the edge that the desination is on
    switch (playerCode)
    {
    case 1:
        return abs(destination.first - i);
        break;

    case 2:
        return abs(destination.second - j);
        break;

    default:
        std::cout << "You done fucked up, using H = 0" << std::endl;
        return 0.0;
        break;
    }
}

int A_star_approach::tracePath(std::vector<std::vector<cell>> cellDetails, std::vector<std::vector<double>> playerCircuit, std::pair<int, int> destination, int *toPlayX, int *toPlayY)
{
    // variables to store the parent coords of a cell
    int row = destination.first;
    int col = destination.second;

    // Stack will contain the cells on the best path
    std::stack<std::pair<int, int>> Path;

    // Iterate from destination tile adding tiles on path to the path
    while (!(cellDetails[row][col].parent_i == row && cellDetails[row][col].parent_j == col))
    {
        // If tile is on a virtual edge for one of the players, don't add it to the path to the real edge
        // Logic could be tidied up here, done in a fugue state
        // switch (playerCode)
        // {
        // case 1:
        //     if ((row == 0 || row == 12) || (playerCircuit[row][col] != 1))
        //     {
        //         // std::cout << "not adding to path" << std::endl;
        //     }
        //     else
        //     {
        //         Path.push(std::make_pair(row, col));
        //     }
        //     break;

        // case 2:
        //     if ((col == 0 || col == 12) || (playerCircuit[row][col] != 1))
        //     {
        //         // std::cout << "not adding to path" << std::endl;
        //     }
        //     else
        //     {
        //         Path.push(std::make_pair(row, col));
        //     }
        //     break;
        // }

        Path.push(std::make_pair(row, col));

        // Change row and col to parents of next tile, until meeting tile which is it's own parent (starting tile)
        int tempRow = cellDetails[row][col].parent_i;
        int tempCol = cellDetails[row][col].parent_j;
        row = tempRow;
        col = tempCol;
    }

    //  Remove from path if contents of circuit at row col is 0?? therefore wouldn't add to path length

    // Path.size() == 0 when tile is already connected to a virtual edge, caused errors so had to implement a check
    if (Path.size() != 0)
    {
        // Take the top tile on the stack, this is next tile to be played
        std::pair<int, int> tileToPlay = Path.top();
        *toPlayX = tileToPlay.first - 1;
        *toPlayY = tileToPlay.second - 1;

        // Iterator to count the length of the path
        int count = 0;

        std::cout << "Path to win (row, column): " << std::endl;
        while (!Path.empty())
        {
            count += 1;
            // Take the top pair on the stack
            std::pair<int, int> p = Path.top();
            // Remove the top pair from the stack
            std::cout << "(" << p.first << ", " << p.second << ")" << std::endl;
            Path.pop();
        }

        return count;

        // If the current path being evaluated is better than the best path so far, set the new best path length
        // if (count < *estimatedWinPathLen)
        // {

        //     // Set the new best path length
        //     *estimatedWinPathLen = count;

        //     *toPlayX = tileToPlay.first - 1; // -1 to "uncorrect" padding offset introduced at top of A_Star()
        //     *toPlayY = tileToPlay.second - 1;
        // }
    }

    return -1;
}

int A_star_approach::A_Star(std::vector<std::vector<double>> playerCircuit, std::pair<int, int> source, std::pair<int, int> destination, int *toPlayX, int *toPlayY)
{

    // Correction factor between the board matrix coords (which is unpadded) to the circuit coords (which are padded) so we can pass in "normal" coords to the function
    // We already pass in coords to the padding so DO NOT correct.
    // std::cout << "Source cell is " << source.first << ", " << source.second << std::endl;
    // source.first = source.first + 1;
    // source.second = source.second + 1;
    // std::cout << "Padded source cell coords are " << source.first << ", " << source.second << std::endl;
    // std::cout << "Targeting " << destination.first << ", " << destination.second << std::endl;#

    std::cout << "Connecting (" << source.first << ", " << source.second << ")" << std::endl;
    std::cout << "to (" << destination.first << ", " << destination.second << ")" << std::endl;

    if (!isCellValid(playerCircuit, source.first, source.second))
    {
        std::cout << "Source tile is not valid" << std::endl;

        return -1;
    }

    if (!isCellValid(playerCircuit, destination.first, destination.second))
    {
        std::cout << "Destination tile is not valid" << std::endl;
        return -1;
    }

    //  This is going to kick up every time because I am trying to connect to occupied tiles, basically redundant
    // if (!isCellOccupied(playerCircuit, source.first, source.second) || !isCellOccupied(playerCircuit, destination.first, destination.second))
    // {
    //     std::cout << "Source or destination tile is occupied" << std::endl;
    //     return;
    // }

    if (isDestination(source.first, source.second, destination))
    {
        std::cout << "Already at destination!" << std::endl;
        return -1;
    }

    // Create a 2D vector full of false's
    std::vector<std::vector<bool>> closedList(playerCircuit.size(), std::vector<bool>(playerCircuit.size(), false));

    // Create a 2D array to store cell data, cannot figure out how to create empty vector of structures of defined size and this works
    // cell cellDetails[playerCircuit.size()][playerCircuit.size()];
    std::vector<std::vector<cell>> cellDetails(playerCircuit.size(), std::vector<cell>(playerCircuit.size()));

    // for (int i = 0; i < int(pow(playerCircuit.size(), 2)); i++)
    // {
    //     cellDetails.push_back({-1, -1, std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max()});
    // }

    // cellDetails.resize(playerCircuit.size(), std::vector<cell>(playerCircuit.size()));

    int i, j;

    // Set blank details for all cells
    for (int i = 0; i < playerCircuit.size(); i++)
    {
        for (int j = 0; j < playerCircuit.size(); j++)
        {
            cellDetails[i][j].f = std::numeric_limits<double>::max();
            cellDetails[i][j].g = std::numeric_limits<double>::max();
            cellDetails[i][j].h = std::numeric_limits<double>::max();
            cellDetails[i][j].parent_i = -1;
            cellDetails[i][j].parent_j = -1;
        }
    }

    // Set details for source cell
    i = source.first, j = source.second;

    cellDetails[i][j].f = 0.0;
    cellDetails[i][j].g = 0.0;
    cellDetails[i][j].h = 0.0;
    cellDetails[i][j].parent_i = i;
    cellDetails[i][j].parent_j = j;

    // Create open list containing <f, <i, j>>
    std::set<std::pair<double, std::pair<int, int>>> openList;
    // Insert starting element into open list
    openList.insert(std::make_pair(0.0, std::make_pair(i, j)));

    bool destFound = false;

    while (!openList.empty())
    {

        std::pair<double, std::pair<int, int>> p;
        p = *openList.begin();
        openList.erase(openList.begin());

        i = p.second.first;
        j = p.second.second;
        closedList[i][j] = true;

        // Neighbours of a given hex:
        // (i - 1, j));
        // (i - 1, j + 1));
        // (i, j - 1));
        // (i, j + 1));
        // (i + 1, j - 1));
        // (i + 1, j));

        std::vector<std::pair<int, int>> neighbours;
        neighbours.push_back(std::make_pair(-1, 0));
        neighbours.push_back(std::make_pair(-1, 1));
        neighbours.push_back(std::make_pair(0, -1));
        neighbours.push_back(std::make_pair(0, 1));
        neighbours.push_back(std::make_pair(1, -1));
        neighbours.push_back(std::make_pair(1, 0));

        double gNew, hNew, fNew;

        for (auto &&neighbour : neighbours)
        {
            // Add neighbour translation values to input cell
            int x = i + neighbour.first;
            int y = j + neighbour.second;

            if (isCellValid(playerCircuit, x, y))
            {

                if (isDestination(x, y, destination))
                {
                    // Set parent of cell to previous cell
                    cellDetails[x][y].parent_i = i;
                    cellDetails[x][y].parent_j = j;
                    // Trace the path from the destination cell to the starting cell
                    int pathLength = tracePath(cellDetails, playerCircuit, destination, toPlayX, toPlayY);
                    destFound = true;
                    return pathLength;
                }

                // Check if cell is on the close list, or if it is already occupied
                else if (!closedList[x][y] && !isCellOccupied(playerCircuit, x, y))
                {
                    // Add previous cells g (distance from source)
                    gNew = cellDetails[i][j].g + playerCircuit[x][y];
                    hNew = calculateHValue(x, y, destination);
                    fNew = gNew + hNew;

                    // If tile isn't in open list (f value is set to max), add it. Make current tile the parent of this neighbour and record new f,g,h.
                    // If tile is in the open list, check if fNew is lower than current one. If yes, replace, if no. do not.
                    if (cellDetails[x][y].f == std::numeric_limits<double>::max() || fNew < cellDetails[x][y].f)
                    {
                        openList.insert(std::pair(fNew, std::make_pair(x, y)));

                        cellDetails[x][y].f = fNew;
                        cellDetails[x][y].g = gNew;
                        cellDetails[x][y].h = hNew;
                        cellDetails[x][y].parent_i = i;
                        cellDetails[x][y].parent_j = j;
                    }
                }
            }
        }
    }
    if (!destFound)
    {
        std::cout << "Couldn't find path to destination, i.e opponent has won" << std::endl;
        return -1;
    }
    return -1;
}

void A_star_approach::playMove(std::vector<std::vector<int>> boardMatrix, int colourCode, int *x, int *y)
{
    updateCircuits(boardMatrix, colourCode);
    showCircuits();

    //  For a given board state, run A* between black sides and white sides. Subtract white side path length from black side path length (number of tiles
    // needed to connect them). That is board score. Use in MiniMax.

    // // Distance between black edges
    // int blackTurnsToWin = A_Star(blackCircuit, std::make_pair(0, floor(blackCircuit.size() / 2)), std::make_pair(blackCircuit.size() - 1, floor(blackCircuit.size() / 2)), x, y);
    // // Distance between white edges
    // int whiteTurnsToWin = A_Star(whiteCircuit, std::make_pair(floor(whiteCircuit.size() / 2), 0), std::make_pair(floor(whiteCircuit.size() / 2), whiteCircuit.size() - 1), x, y);

    // std::cout << "Black turns to win:" << std::endl;
    // std::cout << blackTurnsToWin << std::endl;
    // std::cout << "White turns to win:" << std::endl;
    // std::cout << whiteTurnsToWin << std::endl;

    return;
}

float A_star_approach::EvaluateBoard()
{
    // Define float value to estimate the state of the board
    // Higher values favour black, lower values favour white
    float boardStateScore;

    return 0.0f;
}
