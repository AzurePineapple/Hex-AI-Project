#include <vector>
#include <queue>
#include <map>
#include <set>
#include <stack>
#include <limits>
#include <math.h>
#include <chrono>
#include <algorithm>
#include <random>
#include <memory>
#include <thread>
#include <mutex>
#include <omp.h>

struct TreeNode
{
    std::vector<std::vector<int>> boardMatrix;
    std::vector<std::unique_ptr<TreeNode>> children;
    TreeNode *parent;
    int visits;
    double reward;
    int raveVisits;
    double raveReward;
    int row;
    int col;
    int playerOwner;

    TreeNode(std::vector<std::vector<int>> boardMatrix, int i = -1, int j = -1, TreeNode *parent = nullptr, int whoPlayed = 0)
        : boardMatrix(boardMatrix), parent(parent), visits(0), reward(0.0), raveVisits(0), raveReward(0.0), row(i), col(j), playerOwner(whoPlayed){};

    // Function to get the child of the node that has a matching board state
    TreeNode *findChild(std::vector<std::vector<int>> board)
    {
        for (const auto &child : children)
        {
            if (child->boardMatrix == board)
            {
                return child.get();
            }
        }
        return nullptr;
    }

    // Utility function to display properties of the node
    void show() const
    {
        std::cout << "Visits: " << visits << ", Reward: " << reward << std::endl;
        std::cout << "Row: " << row << ", Col: " << col << std::endl;
        std::cout << "Number of Children: " << children.size() << std::endl;
        if (parent != nullptr)
        {
            std::cout << "Parent row: " << parent->row << ", Parent col: " << parent->col << std::endl;
        }
        else
        {
            std::cout << "Parent: nullptr" << std::endl;
        }
        std::cout << "Board Matrix:" << std::endl;
        int boardSize = boardMatrix.size();
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
};

// Function that throws an error if you attempt to divide by zero
double safeDivide(double numerator, double denominator)
{
    if (denominator == 0.0)
    {
        throw std::runtime_error("Division by zero");
    }
    return numerator / denominator;
}

// Utility function to get all possible moves from a given state
std::vector<std::pair<std::vector<std::vector<int>>, std::pair<int, int>>> getPossibleMoves(const std::vector<std::vector<int>> &boardMatrix, int playerCode)
{
    // Vector to store possible board states after move
    std::vector<std::pair<std::vector<std::vector<int>>, std::pair<int, int>>> availableMoves;
    int boardSize = boardMatrix.size();
    availableMoves.reserve(boardSize * boardSize); // Reserve maximum possible moves

    std::vector<std::vector<int>> newBoard = boardMatrix; // Create a new board state once

    // Iterate over the board
    for (int i = 0; i < boardSize; ++i)
    {
        for (int j = 0; j < boardSize; ++j)
        {
            // Check if the cell is empty, if so fill it with player's piece, add that state to available moves,
            // then empty that board state.
            if (boardMatrix[i][j] == 0)
            {
                newBoard[i][j] = playerCode;
                availableMoves.push_back({newBoard, {i, j}});
                newBoard[i][j] = 0; // Revert the change
            }
        }
    }
    availableMoves.shrink_to_fit(); // Optional: shrink to actual size to free unused memory
    return availableMoves;
}

// Utility function to detect if board state is terminal (there is a winner). Not outright detecting a winner, detetcting if the board is full since a full board has a winner. This hopefully saves time checking for a winner after each move.
bool isTerminal(const std::vector<std::vector<int>> &boardMatrix)
{
    // int boardSize = boardMatrix.size();
    // // Return false if any position in the boardMatrix is unfilled
    // for (int i = 0; i < boardSize; i++)
    // {
    //     for (int j = 0; j < boardSize; j++)
    //     {
    //         if (boardMatrix[i][j] == 0)
    //         {
    //             return false;
    //         }
    //     }
    // }

    // // Return true if board is full (there is always a winner)
    // return true;

    // Alternative method using inbuild efficient functions to accomplish the same tast
    return std::all_of(boardMatrix.begin(), boardMatrix.end(), [](const std::vector<int> &row)
                       { return std::all_of(row.begin(), row.end(), [](int cell)
                                            { return cell != 0; }); });
}

// Gets the 6 neighbours of a given coordinate pair
std::vector<std::pair<int, int>> getNeighbours(int i, int j, int size)
{
    std::vector<std::pair<int, int>> neighbours;
    // To make the DFS prioritise moves in the correct direction first, we need to add those neighbours to the stack last,
    // so we need to add them last here. The "correct" directions are downward directions (increasing row count), so put those last
    std::vector<std::pair<int, int>> directions = {{-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}};

    for (auto &&dir : directions)
    {
        int iNei = i + dir.first;
        int jNei = j + dir.second;
        if (0 <= iNei && iNei < size && 0 <= jNei && jNei < size)
        {
            neighbours.push_back(std::pair<int, int>(iNei, jNei));
        }
    }
    return neighbours;
}

// Redundant
// A breadth first search to determine if black has won the game - we only need this because if black hasn't won, white has.
bool BlackWonBFS(std::vector<std::pair<int, int>> startPositions, std::vector<std::vector<int>> boardMatrix)
{
    int size = boardMatrix.size();
    // Store visited nodes
    std::set<std::pair<int, int>> visited;
    // Create the queue of nodes to explore
    std::queue<std::pair<int, int>> nodeQueue;

    // Add the starting nodes to the queue
    for (auto pos : startPositions)
    {
        visited.insert(pos);
        nodeQueue.push(pos);
    }

    // Do the BFS
    while (!nodeQueue.empty())
    {
        std::pair<int, int> coordPair = nodeQueue.front(); // Grab the first element in the queue (FIFO)
        nodeQueue.pop();
        int x = coordPair.first;
        int y = coordPair.second;

        // Check if the current tile is in the bottom row, in which case black has won
        if (x == size - 1)
        {
            return true;
        }

        // Iterate through the neighbours of the current tile
        for (std::pair<int, int> neiCoords : getNeighbours(x, y, size))
        {
            int nx = neiCoords.first;
            int ny = neiCoords.second;

            // Check if the neighbour is a black tile, and if it is not already in the list of visited nodes
            if (boardMatrix[nx][ny] == 1 && visited.find({nx, ny}) == visited.end())
            {
                // You can use curly braces for pair initialisation I hate this damn language
                visited.insert({nx, ny});
                nodeQueue.push({nx, ny});
            }
        }
    }

    // If queue is exhausted without finding a black win, then return false
    return false;
}

// Thinking about it this may be more efficient for our needs of simply finding an existing path - basically just involved changing a queue to a stack and taking FILO
bool BlackWonDFS(std::vector<std::pair<int, int>> startPositions, std::vector<std::vector<int>> boardMatrix)
{
    int size = boardMatrix.size();
    // Store visited nodes
    std::set<std::pair<int, int>> visited;
    // Create the stack of nodes to explore
    std::stack<std::pair<int, int>> nodeStack;

    // Add the starting nodes to the stack
    for (auto pos : startPositions)
    {
        visited.insert(pos);
        nodeStack.push(pos);
    }

    // Do the DFS
    while (!nodeStack.empty())
    {
        std::pair<int, int> coordPair = nodeStack.top(); // Grab the top element of the stack (FILO)
        nodeStack.pop();
        int x = coordPair.first;
        int y = coordPair.second;

        // Check if the current tile is in the bottom row, in which case black has won
        if (x == size - 1)
        {
            return true;
        }

        // Iterate through the neighbours of the current tile
        for (std::pair<int, int> neiCoords : getNeighbours(x, y, size))
        {
            int nx = neiCoords.first;
            int ny = neiCoords.second;

            // Check if the neighbour is a black tile, and if it is not already in the list of visited nodes
            if (boardMatrix[nx][ny] == 1 && visited.find({nx, ny}) == visited.end())
            {
                // You can use curly braces for pair initialisation I hate this damn language
                visited.insert({nx, ny});
                nodeStack.push({nx, ny});
            }
        }
    }

    // If stack is exhausted without finding a black win, then return false
    return false;
}

// Returns 1.0 if black won the board
double getResult(std::vector<std::vector<int>> boardMatrix)
{
    // Throw an error if called on a nor-terminal board
    if (!isTerminal(boardMatrix))
    {
        throw std::logic_error("getResult called on a non-terminal state");
    }

    int boardSize = boardMatrix.size();

    // Find all tiles in top row that contain black pieces, add them to a vector
    std::vector<std::pair<int, int>> startPositions;
    for (int col = 0; col < boardSize; col++)
    {
        if (boardMatrix[0][col] == 1)
        {
            startPositions.push_back({0, col});
        }
    }

    // If the top row has 0 black pieces, white has automatically won, and we can skip the pathfinding algo
    bool blackWon;
    if (!startPositions.empty())
    {
        // Testing determined DFS ~30ms faster on average (See sticky note)
        // blackWon = BlackWonBFS(startPositions, boardMatrix);
        blackWon = BlackWonDFS(startPositions, boardMatrix);
    }
    else
    {
        blackWon = false;
    }

    return blackWon;
}

// Returns the classic UCT value of a node
double getUCTValue(TreeNode *node, TreeNode *child)
{
    double C = 1.0; // exploration constant
    // Add a small value to the number of visits to avoid division by 0 error, and ensure that the division gives a large value as a result
    double UCT = (child->reward / (child->visits + 1e-8)) +
                 C * sqrt((2 * log(node->visits)) / (child->visits + 1e-8));
    return UCT;
}

// Returns the UCT-RAVE value of the node
double getUCTRAVEValue(TreeNode *node, TreeNode *child)
{
    double C = 1.0; // exploration constant
    double b = 0.5; // Parameter in beta parameter, empirically determined, using value from https://papersdb.cs.ualberta.ca/~papersdb/uploaded_files/1029/paper_thesis.pdf

    double childVisits = child->visits + 1e-8;
    double raveVisits = child->raveVisits + 1e-8;
    double beta = safeDivide(raveVisits, (childVisits + raveVisits + 4 * b * b * childVisits * raveVisits));
    double childValue = safeDivide(child->reward, childVisits);
    double raveValue = safeDivide(child->raveReward, raveVisits);
    double score = (1 - beta) * childValue + beta * raveValue;
    score += C * sqrt(safeDivide((2 * log(node->visits)), childVisits));

    return score;
}

TreeNode *selectChild(TreeNode *node)
{
    std::vector<TreeNode *> bestChildren; // Vector of best UCT scores
    double bestValue = -std::numeric_limits<double>::infinity();

    // Iterate through node's children and identify those with highest scores
    for (auto &child : node->children)
    {
        double nodeScore = getUCTRAVEValue(node, child.get());

        if (nodeScore > bestValue)
        {
            bestValue = nodeScore;
            bestChildren.clear();
            bestChildren.push_back(child.get());
        }
        else if (nodeScore == bestValue)
        {
            bestChildren.push_back(child.get());
        }
    }

    // Return a random best child
    if (!bestChildren.empty())
    {
        int randIndex = rand() % bestChildren.size();
        return bestChildren[randIndex];
    }
    else
    {
        std::cerr << "Error: No best children found\n";
        // Return nullptr in case anything breaks
        return nullptr;
    }
}

// Function to expand all possible children of a node
void expand(TreeNode *node, int playerCode)
{
    // Get available moves from node's state
    std::vector<std::pair<std::vector<std::vector<int>>, std::pair<int, int>>> availableMoves = getPossibleMoves(node->boardMatrix, playerCode);
    if (availableMoves.empty())
    {
        throw new std::logic_error("Expand called on terminal board");
    }

    // Reserve space for child nodes using size of availableMoves
    node->children.reserve(availableMoves.size());

    // Iterate through the available moves
    for (auto &&move : availableMoves)
    {
        // Break down each move in the vector of pairs because it'd get messy otherwise
        std::vector<std::vector<int>> state = std::move(move.first); // Using move to prevent copying, for speed
        int row = move.second.first;
        int col = move.second.second;

        // Create a new child, passing the new boardMatrix and the current node to the constructor
        // TreeNode *child = new TreeNode(state, row, col, node);
        auto child = std::make_unique<TreeNode>(std::move(state), row, col, node, playerCode);

        // Add the new node as a child of the current node
        node->children.push_back(std::move(child));
    }
}

std::pair<bool, std::vector<std::pair<int, int>>> simulate(const std::vector<std::vector<int>> &boardMatrix, int playerCode)
{
    // Get available moves
    std::vector<std::pair<std::vector<std::vector<int>>, std::pair<int, int>>> availableMoves = getPossibleMoves(boardMatrix, playerCode);
    // Work out how many moves the playerCode needs to make (half rounded up) - making sure to cast to double first
    double noAvailableMoves = availableMoves.size();
    int noMovesToSelect = ceil(noAvailableMoves / 2);
    int boardSize = boardMatrix.size();
    int opponentPlayerCode = (playerCode == 1) ? 2 : 1; // Get player code of the opponent

    // Pre-allocate memory for efficiency
    std::vector<std::pair<int, int>> moves;
    moves.reserve(noAvailableMoves);

    // Seed and generate a random number
    std::random_device rd;
    std::mt19937 gen(rd());

    // Shuffle the available moves
    std::shuffle(availableMoves.begin(), availableMoves.end(), gen);

    auto boardMatrixCopy = boardMatrix;

    // For the first noMovesToSelect moves, set board position to playerCode, for the rest set to opponent
    for (int i = 0; i < noAvailableMoves; i++)
    {
        const auto &moveCoords = availableMoves[i].second;

        if (i < noMovesToSelect)
        {
            boardMatrixCopy[moveCoords.first][moveCoords.second] = playerCode;
        }
        else
        {
            boardMatrixCopy[moveCoords.first][moveCoords.second] = opponentPlayerCode;
        }

        moves.push_back(moveCoords);
    }

    // Throw a hissy fit if the above didn't work
    if (!isTerminal(boardMatrixCopy))
    {
        throw std::logic_error("Non terminal board supplied at end of simulation");
    }

    // Return the result of the (terminal) board state
    bool blackWon = getResult(boardMatrixCopy);
    return {blackWon, moves};
}

void backpropagate(TreeNode *node, bool blackWon, std::vector<std::pair<int, int>> moves)
{
    // Iterate back up the tree until you hit the root node
    while (node != nullptr)
    {

        // Determine the result based on the current node's playerOwner
        double result;
        if (node->playerOwner == 1)
        { // If the node is owned by the black player
            result = blackWon ? 1.0 : 0.0;
        }
        else if (node->playerOwner == 2)
        { // If the node is owned by the white player
            result = blackWon ? 0.0 : 1.0;
        }
        else
        {                 // If the node's playerOwner is not defined (root case)
            result = 0.0; // This is arbitrary as it shouldn't affect the outcome
        }

        // Increment the number of visits and the reward of the node
        node->visits++;
        node->reward += result;

        // Update RAVE statistics
        for (const auto &childPtr : node->children)
        {
            TreeNode *child = childPtr.get();

            for (const auto &move : moves)
            {
                if (child->row == move.first && child->col == move.second)
                {
                    child->raveVisits++;
                    child->raveReward += result;
                }
            }
        }

        node = node->parent;
        result = 1 - result;
    }
}

std::pair<std::vector<bool>, std::vector<std::vector<std::pair<int, int>>>> parallelSimulate(const std::vector<std::vector<int>> &boardMatrix, int playerCode, int numberOfThreads)
{
    // Create a vector of threads and two vectors to store the results and the moves
    std::vector<std::thread> threads;
    std::vector<bool> blackWins(numberOfThreads);
    std::vector<std::vector<std::pair<int, int>>> resultMoves(numberOfThreads);
    for (int threadIndex = 0; threadIndex < numberOfThreads; threadIndex++)
    {
        threads.push_back(std::thread([&, threadIndex]()
                                      { auto simResult = simulate(boardMatrix, playerCode);
                                      blackWins[threadIndex] = simResult.first;
                                      resultMoves[threadIndex] = simResult.second; }));
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    return {blackWins, resultMoves};
}

std::unique_ptr<TreeNode> MCTSleaf(TreeNode *root, int playerCode, int iterations, double timeLimit, bool isParallelised, bool maxVisitsChild)
{
    auto startTime = std::chrono::steady_clock::now();
    int numberOfThreads = 1;
    if (isParallelised)
    {
        numberOfThreads = std::thread::hardware_concurrency();
        // std::cout << "Number of threads used: " << numberOfThreads << std::endl;
    }

    for (int i = 0; i < iterations; i++)
    {
        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsedTime = currentTime - startTime;
        if (elapsedTime.count() > timeLimit)
        {
            if (isParallelised)
            {
                std::cout << "Time limit of " << timeLimit << " seconds reached, performed " << root->visits << " iterations." << std::endl;
            }
            else
            {
                std::cout << "Time limit of " << timeLimit << " seconds reached, performed " << i << " iterations." << std::endl;
            }
            break;
        }

        TreeNode *node = root; // Get pointer to root node
        int currentPlayerCode = playerCode;

        // Selection
        while (!node->children.empty() && !isTerminal(node->boardMatrix))
        {
            node = selectChild(node);
            if (node == nullptr)
            {
                throw std::logic_error("Selected nullptr");
            }

            currentPlayerCode = (currentPlayerCode == 1) ? 2 : 1;
        }

        // Expansion
        if (!isTerminal(node->boardMatrix))
        {
            expand(node, currentPlayerCode);
            if (!node->children.empty())
            {
                int randChildIndex = rand() % node->children.size();
                node = node->children[randChildIndex].get(); // Get raw pointer to random child
            }
            currentPlayerCode = (currentPlayerCode == 1) ? 2 : 1;
        }

        if (isParallelised)
        {
            // Returns the result of the simulation, and the moves taken in that simulation
            std::pair<std::vector<bool>, std::vector<std::vector<std::pair<int, int>>>> parallelResults = parallelSimulate(node->boardMatrix, currentPlayerCode, numberOfThreads);

            std::vector<bool> blackWins = parallelResults.first;
            std::vector<std::vector<std::pair<int, int>>> simMoves = parallelResults.second;

            for (size_t i = 0; i < blackWins.size(); i++)
            {
                // Pass in the node, the result of each parallel sim, and the moves of each parallel sim
                backpropagate(node, blackWins[i], simMoves[i]);
            }
        }
        else
        {
            // Simulation
            std::pair<bool, std::vector<std::pair<int, int>>> sequentialResults = simulate(node->boardMatrix, currentPlayerCode); // Returns true if black wins
            bool blackWon = sequentialResults.first;
            std::vector<std::pair<int, int>> moves = sequentialResults.second;
            // Backpropagation
            backpropagate(node, blackWon, moves);
        }
    }

    // Determine best child based on visits
    std::unique_ptr<TreeNode> bestChild = nullptr;
    int maxVisits = -1;
    double maxWinRate = -1;
    if (root->children.empty())
    {
        throw new std::logic_error("Root has no children");
    }

    for (auto &childPtr : root->children)
    {
        if (maxVisitsChild)
        {
            // Max Visits Logic (Robust Child)
            if (childPtr->visits > maxVisits)
            {
                bestChild = std::move(childPtr);
                maxVisits = bestChild->visits;
            }
        }
        else
        {
            // Highest winrate logic
            double winrate = childPtr->reward / childPtr->visits;
            if (winrate > maxWinRate)
            {
                bestChild = std::move(childPtr);
                maxWinRate = winrate;
            }
        }
    }
    return bestChild; // Return the best child node
}