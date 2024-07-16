#include "Player.h"

Player::Player()
{
}

void Player::createPlayer(std::string playerType, int colourCode, std::string AIType = "none", int size = -1, bool isParallelisedParam = false, int searchDepthParam = 3, int mctsTimeLimitParam = 10, int mctsIterLimitParam = 10000)
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
        mctsTimeLimit = mctsTimeLimitParam;
        mctsIterLimit = mctsIterLimitParam;
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

        auto minimax_pair = minimaxEngine.go(boardMatrix, blacksTurn, minimaxSearchDepth);
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

        // Second, get the best child of the new board position
        std::unique_ptr<TreeNode> bestChild = MCTS(root.get(), playerColourCode, 100000, 5.0, isParallelised);

        // std::cout << "Selected best child:" << std::endl;
        // bestChild->show();

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