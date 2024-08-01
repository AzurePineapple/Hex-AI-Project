#include "Player.h"

Player::Player()
{
}

void Player::createPlayer(std::string playerType, int colourCode, std::string AIType = "none", int size = -1, bool isParallelisedParam = false, int searchDepthParam = 3, int mmTimeLimitParam = 3, int mctsTimeLimitParam = 10, int mctsIterLimitParam = 10000, bool rootParallelisedParam = true)
{
    if (playerType == "human")
    {
        isComputer = false;
    }

    else if (playerType == "computer")
    {
        isComputer = true;
        AIModel = AIType;
        isParallelised = isParallelisedParam;
        minimaxSearchDepth = searchDepthParam;
        mmTimeLimit = mmTimeLimitParam;
        mctsTimeLimit = mctsTimeLimitParam;
        mctsIterLimit = mctsIterLimitParam;
        rootParallelised = rootParallelisedParam;

        if (AIModel == "minimax")
        {
            minimaxEngine = MiniMax();
            // MiniMax *test = new MiniMax();
        }
        else if (AIModel == "mcts")
        {
            if (size != -1)
            {
                std::vector<std::vector<int>> emptyBoard;
                emptyBoard.resize(size, std::vector<int>(size, 0));
                root = std::make_unique<TreeNode>(emptyBoard);
            }
            else
            {
                throw std::logic_error("Passed incorrect size into root creation");
            }
        }
    }

    else
    {
        std::cout << "Error: Invalid player type" << std::endl;
    }

    if (colourCode == 1)
    {
        playerColour = "black";
        playerColourCode = colourCode;
    }
    else if (colourCode == 2)
    {
        playerColour = "white";
        playerColourCode = colourCode;
    }

    else
    {
        std::cout << "Error: Invalid player colour, defaulting to black" << std::endl;
        playerColour = "black";
    }
}

int *Player::playMove(int x, int y)
{
    if (!isComputer)
    {
        move[0] = x;
        move[1] = y;
        return move;
    }
    else
    {
        std::cout << "something fucked up" << std::endl;
        std::cout << "tried to play human move as computer" << std::endl;
        move[0] = 0;
        move[1] = 0;
        return move;
    }
}

void Player::playMoveAI(std::vector<std::pair<int, int>> availableTiles, int boardSize, int *x, int *y, std::vector<std::vector<int>> boardMatrix)
{
    if (AIModel == "random")
    {
        bool validMove = false;
        while (!validMove)
        {
            *x = rand() % boardSize;
            *y = rand() % boardSize;

            std::pair<int, int> movePair = std::make_pair(*x, *y);

            if (std::find(availableTiles.begin(), availableTiles.end(), movePair) != availableTiles.end())
            {
                validMove = true;
            }
            else
            {
                // std::cout << "Invalid Move" << std::endl;
            }
        }
    }
    else if (AIModel == "minimax")
    {
        bool blacksTurn;
        if (playerColourCode == 1)
        {
            blacksTurn = true;
        }
        else
        {
            blacksTurn = false;
        }

        auto minimax_pair = minimaxEngine.go(boardMatrix, blacksTurn, minimaxSearchDepth, mmTimeLimit);
        *x = minimax_pair.first;
        *y = minimax_pair.second;
    }
    else if (AIModel == "mcts")
    {
        // First step, update the root based on the opponent's move

        // Iterate through all children of the previous root to find the one corresponding to the opponent's move
        auto matchingChild = root->findChild(boardMatrix);

        // Detect if there is indeed a matching child
        if (matchingChild)
        {
            for (auto &child : root->children)
            {
                if (child->boardMatrix == boardMatrix)
                {
                    root = std::move(child);
                    root->parent = nullptr; // Ensure setting this to avoid segmentation fault
                    break;
                }
            }
        }
        else
        {
            auto newChild = std::make_unique<TreeNode>(boardMatrix, -1, -1, nullptr, 3 - playerColourCode); // Passing in opponent's colour code so it can be correctly updated (even though this is not strictly necessary - I don't technically care what the score of the root node is, only its children)
            root = std::move(newChild);
            root->parent = nullptr;
        }

        // Choose between root and leaf parallelisation
        if (rootParallelised)
        {
            // Root Parallelisation method

            // Create as many threads as possible
            const int num_threads = std::thread::hardware_concurrency();
            std::vector<std::thread> threads;

            // Clear the global localRootVector so that it can't be used by an opponent
            for (auto localRoot : localRootVector)
            {
                delete localRoot;
            }

            for (int i = 0; i < num_threads; i++)
            {
                threads.emplace_back(mctsThread, root->boardMatrix, root->row, root->col, root->parent, root->playerOwner, playerColourCode, mctsIterLimit, mctsTimeLimit);
            }

            // Join Threads
            for (auto &thread : threads)
            {
                thread.join();
            }

            // Create vector to store the data for each child, grouped by child row and column
            std::map<std::pair<int, int>, std::vector<int>> childValues;

            // Iterate through all the children of all the localRoots
            for (auto &&localRoot : localRootVector)
            {
                for (auto &&child : localRoot->children)
                {
                    // Create a vector of values, some may be unnecessary but include them for alternate bestChild selection metrics
                    std::vector<int> values = {child->visits,
                                               child->reward,
                                               child->raveVisits,
                                               child->raveReward};

                    // Check if child not in map already
                    if (childValues.find({child->row, child->col}) == childValues.end())
                    {
                        // If not, add the key to the map with the values
                        childValues[{child->row, child->col}] = values;
                    }
                    else
                    {
                        // If so, add the new values to the ones stored in the map
                        for (size_t i = 0; i < values.size(); i++)
                        {
                            childValues[{child->row, child->col}][i] += values[i];
                        }
                    }
                }
            }

            // Get child with the most visits
            int maxValue = 0;
            std::pair<int, int> maxKey;
            for (const auto &entry : childValues)
            {
                const std::vector<int> &vec = entry.second;
                if (vec[0] > maxValue)
                {
                    maxValue = vec[0];
                    maxKey = entry.first;
                }
            }

            std::cout << "(Root) Best child had " << maxValue << " visits" << std::endl;

            // Play the move by setting pointer values
            *x = maxKey.first;
            *y = maxKey.second;

            // No need to update root to be the best child node, as node's children can't be preserved anyway
        }
        else
        {
            // Leaf Parallelisation method

            // Second, get the best child of the new board position
            std::unique_ptr<TreeNode> bestChild = MCTSleaf(root.get(), playerColourCode, mctsIterLimit, mctsTimeLimit, isParallelised, true);

            std::cout << "(Leaf) Best child had " << bestChild->visits << " visits" << std::endl;

            // Third, play the move by setting pointer values
            *x = bestChild->row;
            *y = bestChild->col;

            // Last, set the new root as the return from the algorithm, i.e. step onto the new branch
            // Also set its parent to nullptr
            root = std::move(bestChild);
            root->parent = nullptr;
            // root.get()->show();
        }
    }
}

std::string Player::getColour()
{
    return playerColour;
}

int Player::getColourCode()
{
    return playerColourCode;
}

void Player::updateColourCode(int colourCode)
{
    switch (colourCode)
    {
    case 1:
        playerColour = "black";
        playerColourCode = colourCode;
        break;

    case 2:
        playerColour = "white";
        playerColourCode = colourCode;
        break;

    default:
        std::cout << "Error: Invalid player colour, defaulting to black" << std::endl;
        playerColour = "black";
        break;
    }
}

bool Player::getIsComputer()
{
    return isComputer;
}

Player::~Player()
{
    for (auto localRoot : localRootVector)
    {
        delete localRoot;
    }
}

// Thread function
void mctsThread(std::vector<std::vector<int>> boardMatrix, int row, int col, TreeNode *parent, int playerOwner, int playerColourCode, int mctsIterLimit, double mctsTimeLimit)
{
    // Create a local copy of the root so as to not fuck up the tree during parallel processing
    // Cannot preserve child data from previous runs because of unique pointer limitations.
    TreeNode *localRoot = new TreeNode(boardMatrix, row, col, parent, playerOwner);

    // Perform the mcts search
    // Modifies localRoot in place so no need to return anything
    MCTSroot(localRoot, playerColourCode, mctsIterLimit, mctsTimeLimit);

    // Add the result of the mcts search to a vector, using a mutex to ensure no data corruption
    mtx.lock();
    localRootVector.push_back(localRoot);
    mtx.unlock();
}